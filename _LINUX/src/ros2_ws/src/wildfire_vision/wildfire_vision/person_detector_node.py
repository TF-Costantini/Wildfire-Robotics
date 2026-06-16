"""
person_detector_node.py — Wildfire Robotics UGV
Nodo per il rilevamento persone usando YOLOv8n.

Input:  /camera/image_raw  (sensor_msgs/Image)
Output: /vision/person   (Detection)

Logica:
  1. BGR → inference YOLOv8n
  2. Filtra classe person (COCO=0), confidence > 0.5
  3. Seleziona la bbox con area maggiore (persona più vicina)
  4. Pubblica Detection con centroide + area
  5. Se nessuna persona → found=False
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
import numpy as np
import cv2
import time
import threading

from .WebSocketClient import WebsocketClient
from wildfire_msgs.msg import Detection, Mode


def convert_ros_msg_to_bytes(msg: Image) -> bytes:
    # Example for a raw image array or compressed image string:
    return bytes(msg.data)


class PersonDetectorNode(Node):
    COCO_PERSON_CLASS = 0

    def __init__(self):
        super().__init__('person_detector_node')

        # --- Parametri ---
        self.declare_parameter('camera_topic', '/camera/image_raw')
        self.declare_parameter('inference_server', "ws://localhost:8765")

        self.mode_topic = "/mode"
        self.camera_subscription = None

        self.camera_topic = self.get_parameter('camera_topic').value
        self.inference_server_addr = self.get_parameter('inference_server').value

        # --- Subscriber ---
        self.create_subscription(
            Mode,
            self.mode_topic,
            self._mode_callback,
            10
        )

        # --- Publisher ---
        self._pub = self.create_publisher(Detection, '/vision/person', 10)

        # WEBSOCKET TO INFERENCE

        self.yolo_client = WebsocketClient(
            server_url=self.inference_server_addr,
            callback=self._inference_callback,   # runs when a result comes back
            on_disconnect=self._on_disconnect    # runs if the socket drops while waiting
        )

        # --- Stato gating richieste ---
        # STRETTO: una sola richiesta "in volo" alla volta. La prossima parte
        # SOLO dopo aver ricevuto un risultato (ok o errore). Nessun timeout di
        # reinvio: due richieste contemporanee farebbero arrivare risultati di
        # frame vecchi (la causa dei "dati di inferenza vecchi").
        self._state_lock = threading.Lock()
        self.latest_image = None      # frame più recente in attesa di invio
        self._inflight = False        # True = richiesta inviata, risultato non ancora arrivato


    def _start_camera_processing(self):
        if self.camera_subscription is not None:
            return

        self.yolo_client.connect()

        self.camera_subscription = self.create_subscription(
            Image,
            self.camera_topic,
            self.receive_image,
            10
        )

    def _stop_camera_processing(self):
        if self.camera_subscription is None:
            return

        self.yolo_client.disconnect()
        self.destroy_subscription(self.camera_subscription)
        self.camera_subscription = None

    # ─── Callback principale ─────────────────────────────────────────────────

    def receive_image(self, msg: Image):
        # Tieni sempre solo il frame più recente; l'invio è gestito da _dispatch.
        with self._state_lock:
            self.latest_image = msg
        self._dispatch()

    def _dispatch(self):
        """Invia il frame più recente SOLO se non c'è già una richiesta in volo.

        Chiamato dal thread ROS (receive_image) e dal thread WebSocket
        (_on_response_done / _on_disconnect): il lock garantisce un solo invio
        concorrente e un solo frame in volo.
        """
        with self._state_lock:
            if self._inflight:
                return  # STRETTO: aspetta il risultato precedente, nessun reinvio
            img = self.latest_image
            self.latest_image = None
            if img is None:
                return
            self._inflight = True

        if not self.send_to_inference_server(img):
            # Invio fallito: nessun risultato arriverà → libera il gate.
            with self._state_lock:
                self._inflight = False

    def _on_response_done(self):
        """Risultato ricevuto (ok o errore): libera il gate e invia il prossimo frame."""
        with self._state_lock:
            self._inflight = False
        self._dispatch()

    def _on_disconnect(self):
        """Socket caduto mentre si attendeva un risultato: libera il gate, altrimenti
        resterebbe bloccato per sempre (il risultato non arriverà mai)."""
        with self._state_lock:
            self._inflight = False

    def send_to_inference_server(self, msg: Image) -> bool:
        try:
            # 1. Converti i byte grezzi di ROS in una matrice NumPy (immagine specchio)
            # Nota: assumendo che l'immagine sia rgb8 o bgr8 (3 canali)
            frame = np.frombuffer(msg.data, dtype=np.uint8).reshape(msg.height, msg.width, 3)

            # Se la tua telecamera ROS pubblica in RGB, convertila in BGR per OpenCV
            if msg.encoding == 'rgb8':
                frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)

            # 2. Comprimi la matrice in un buffer JPEG (riduce il peso da 2MB a ~150KB!)
            success, encoded_image = cv2.imencode('.jpg', frame, [int(cv2.IMWRITE_JPEG_QUALITY), 90])

            if not success:
                print("[ROS Node] Errore durante la codifica JPEG")
                return False

            # 3. Trasforma il buffer in byte e sparalo sul WebSocketClient
            jpeg_bytes = encoded_image.tobytes()
            return self.yolo_client.push_image(jpeg_bytes)

        except Exception as e:
            print(f"[ROS Node] Errore nel processing dell'immagine: {e}")
            return False

    def _mode_callback(self, mode_msg: Mode):
        if mode_msg.mode == Mode.FOLLOW:
            self._start_camera_processing()
            return
        self._stop_camera_processing()

    def _inference_callback(self, result: dict):
        status = result["status"]

        if status != "ok":
            self.get_logger().info(f"ERROR while processing YOLO Inference...")
            # Libera comunque il gate e invia il prossimo frame.
            self._on_response_done()
            return

        # self.get_logger().info(f"New YOLO Results Received...")
        detection = result["detection"]
        if detection is None:
            self._publish_detection(False, 0.0, 0.0, 0.0, 0, 0, 0.0)
        else:
            self._publish_detection(
                True,
                detection["cx"],
                detection["cy"],
                detection["area"],
                detection["img_w"],
                detection["img_h"],
                detection["conf"]
            )

        # Risposta gestita: sblocca il gate e invia il frame più recente.
        self._on_response_done()

    # ─── Publishing ─────────────────────────────────────────────────────────

    def _publish_detection(self, found: bool, cx: float, cy: float,
                          area: float, img_w: int, img_h: int,
                          confidence: float):
        """Pubblica un messaggio Detection."""
        msg = Detection()
        msg.found = found
        msg.cx = cx
        msg.cy = cy
        msg.area = area
        msg.img_w = float(img_w)
        msg.img_h = float(img_h)
        msg.confidence = confidence
        msg.use_confidence = True
        msg.timestamp = time.time_ns()

        self._pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = PersonDetectorNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
