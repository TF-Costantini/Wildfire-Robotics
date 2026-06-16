import threading
import json
import time
from typing import Callable, Optional
from websockets.sync.client import connect as ws_connect
from websockets.exceptions import ConnectionClosed

class WebsocketClient:
    def __init__(self, server_url: str = "ws://localhost:8765",
                 callback: Optional[Callable[[dict], None]] = None,
                 on_disconnect: Optional[Callable[[], None]] = None):
        self.server_url = server_url
        self.callback = callback
        self.on_disconnect = on_disconnect  # notificato quando il socket cade

        self.ws = None
        self._thread: Optional[threading.Thread] = None
        self._running = False
        self._lock = threading.Lock()  # Protegge l'invio di frame concorrenti da ROS 2

    def connect(self):
        """Stabilisce la connessione nativa bloccante e avvia il thread di ascolto."""
        if self.ws is not None:
            return  # Già connesso

        print(f"[YOLO Client] Tentativo di connessione a {self.server_url}...")
        while True:
            try:
                # La nuova libreria blocca qui finché non è connessa al 100%
                self.ws = ws_connect(self.server_url, max_size=None)
                print("[YOLO Client] Connessione WebSocket stabilita con successo!")
                break
            except Exception as e:
                print(f"[YOLO Client] Server YOLO non raggiungibile ({e}). Riprovo tra 2 secondi...")
                self.ws = None
                time.sleep(2)

        # Avvia il ciclo di ricezione in un thread separato per non bloccare ROS 2
        self._running = True
        self._thread = threading.Thread(target=self._listen_loop, daemon=True)
        self._thread.start()

    def disconnect(self):
        """Chiude la connessione in modo pulito e termina il thread."""
        self._running = False
        if self.ws:
            try:
                self.ws.close()
            except Exception:
                pass
            self.ws = None

        if self._thread and self._thread.is_alive():
            self._thread.join(timeout=2.0)
        print("[YOLO Client] Disconnesso.")

    def push_image(self, image_bytes: bytes) -> bool:
        """Invia i byte dell'immagine direttamente dal callback di ROS 2 (Thread-safe).

        Ritorna True se l'invio è riuscito, False altrimenti (socket chiuso o
        connessione persa). Il chiamante usa l'esito per gestire il gate.
        """
        if self.ws is None:
            print("[YOLO Client] Errore: Impossibile inviare frame, websocket chiuso.")
            return False

        try:
            with self._lock:
                self.ws.send(image_bytes)
            return True
        except (ConnectionClosed, Exception) as e:
            print(f"[YOLO Client] Connessione persa durante l'invio: {e}")
            self._handle_disconnection()
            return False

    # ----------------------------------------------------------------
    # Meccanica di Background (Thread Sincrono Nativo)
    # ----------------------------------------------------------------

    def _listen_loop(self):
        """Riceve continuamente i dati di inferenza dal server YOLO."""
        print("[YOLO Client] Thread di ascolto avviato.")
        while self._running and self.ws is not None:
            try:
                # Lettura bloccante nativa sincrona
                message = self.ws.recv()
                if self.callback and message:
                    try:
                        parsed_data = json.loads(message)
                        self.callback(parsed_data)
                    except Exception as callback_err:
                        print(f"[YOLO Client] Errore nel callback di ROS 2: {callback_err}")
            except ConnectionClosed:
                print("[YOLO Client] Il server YOLO ha chiuso la connessione.")
                break
            except Exception as e:
                if self._running:
                    print(f"[YOLO Client] Errore nel ciclo di ricezione: {e}")
                break

        if self._running:
            self._handle_disconnection()

    def _handle_disconnection(self):
        """Pulisce il vecchio socket e lancia il thread di riconnessione."""
        if self.ws:
            try:
                self.ws.close()
            except Exception:
                pass
            self.ws = None

        # Sblocca il gate del nodo: una richiesta in volo non riceverà mai risposta.
        if self.on_disconnect:
            try:
                self.on_disconnect()
            except Exception:
                pass

        if self._running:
            print("[YOLO Client] Avvio tentativo di riconnessione automatica...")
            # Rilancia connect() che gestirà il loop di retry da 2 secondi
            self.connect()