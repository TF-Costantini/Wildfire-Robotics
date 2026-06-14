import asyncio
import threading
import json
from typing import Callable, Optional
import websockets

class WebsocketClient:
    def __init__(self, server_url: str = "ws://localhost:8765", callback: Optional[Callable[[dict], None]] = None):
        self.server_url = server_url
        self.callback = callback

        self.ws: Optional[websockets.WebSocketClientProtocol] = None
        self._loop: Optional[asyncio.AbstractEventLoop] = None
        self._thread: Optional[threading.Thread] = None
        self._listen_task: Optional[asyncio.Task] = None

    def connect(self):
        """Starts a background thread with an event loop and connects to the server."""
        if self._thread and self._thread.is_alive():
            print("[YOLO Client] Already connected or connecting...")
            return

        # We spin up a dedicated background thread to run the asyncio loop
        # so it doesn't block your ROS2 executor/main thread.
        self._thread = threading.Thread(target=self._run_event_loop, daemon=True)
        self._thread.start()

        # Wait until the loop and connection are established in the background
        while self.ws is None or not self.ws.open:
            pass
        print(f"[YOLO Client] Connected to {self.server_url}")

    def disconnect(self):
        """Closes the connection and cleanly shuts down the background thread."""
        if self._loop and self._loop.is_running():
            asyncio.run_coroutine_threadsafe(self._async_disconnect(), self._loop)
            if self._thread:
                self._thread.join(timeout=2.0)
        print("[YOLO Client] Disconnected.")

    def push_image(self, image_bytes: bytes):
        """Thread-safe method to fire bytes over the socket from ROS2 callback."""
        if not self.ws or not self.ws.open:
            print("[YOLO Client] Error: Cannot push image, websocket is closed.")
            return

        # Safely schedules the async send on our dedicated background loop
        asyncio.run_coroutine_threadsafe(self.ws.send(image_bytes), self._loop)

    # ----------------------------------------------------------------
    # Internal Async / Threading Mechanics
    # ----------------------------------------------------------------

    def _run_event_loop(self):
        """Target for background thread: creates loop and runs it forever."""
        self._loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self._loop)

        # Run the initial connection setup inside the loop
        self._loop.run_until_complete(self._async_connect())
        # Keep the loop alive to handle scheduled send/receive tasks
        self._loop.run_forever()

    async def _async_connect(self):
        """Internal async connector that triggers the listening loop task."""
        try:
            self.ws = await websockets.connect(self.server_url)
            # Spawn the listener loop as an independent task on this loop
            self._listen_task = asyncio.create_task(self._listen_loop())
        except Exception as e:
            print(f"[YOLO Client] Connection failed: {e}")

    async def _listen_loop(self):
        """Continuously listens for incoming inference data from the server."""
        try:
            async for message in self.ws:
                if self.callback:
                    try:
                        parsed_data = json.loads(message)
                        # Fire the user's callback function with the data
                        self.callback(parsed_data)
                    except Exception as callback_err:
                        print(f"[YOLO Client] Error in callback execution: {callback_err}")
        except websockets.exceptions.ConnectionClosed:
            print("[YOLO Client] Connection closed by remote server.")
        except Exception as e:
            print(f"[YOLO Client] Error in listen loop: {e}")

    async def _async_disconnect(self):
        """Internal async teardown."""
        if self._listen_task:
            self._listen_task.cancel()
        if self.ws:
            await self.ws.close()
        if self._loop:
            self._loop.stop()