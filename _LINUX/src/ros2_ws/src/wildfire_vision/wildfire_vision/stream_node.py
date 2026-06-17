"""
stream_node.py — Wildfire Robotics UGV
Nodo che inoltra i frame grezzi della camera al WebSocket di passthrough
(:8766) del server di inferenza, per la web UI di debug (/raw_stream).

Input:  /camera/image_raw  (sensor_msgs/Image)
Output: WebSocket binario JPEG verso ws://<host>:8766 (nessuna risposta)

Differenze rispetto a person_detector_node:
  - NON gated sulla modalità: trasmette sempre (debug feed continuo).
  - Pipe a senso unico: il server :8766 non risponde, quindi niente
    callback/gate "in volo". Fire-and-forget di ogni frame a 15 fps.
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
import numpy as np
import cv2
import threading

from .WebSocketClient import WebsocketClient


class StreamNode(Node):
    def __init__(self):
        super().__init__('stream_node')

        # --- Parametri ---
        self.declare_parameter('camera_topic', '/camera/image_raw')
        self.declare_parameter('raw_stream_server', 'ws://localhost:8766')

        self.camera_topic = self.get_parameter('camera_topic').value
        self.stream_server_addr = self.get_parameter('raw_stream_server').value

        # --- WebSocket di passthrough (senso unico, nessun callback) ---
        self.stream_client = WebsocketClient(
            server_url=self.stream_server_addr,
            callback=None,        # :8766 non risponde
            on_disconnect=None
        )
        # connect() blocca con retry ogni 2 s finché il server non è su:
        # avvialo in un thread per non bloccare l'init del nodo.
        threading.Thread(target=self.stream_client.connect, daemon=True).start()

        # --- Subscriber camera ---
        self.create_subscription(
            Image,
            self.camera_topic,
            self.receive_image,
            10
        )

        self.get_logger().info(
            f'StreamNode avviato: {self.camera_topic} → {self.stream_server_addr}'
        )

    def receive_image(self, msg: Image):
        """Codifica il frame in JPEG e lo spara sul WebSocket di passthrough.

        Fire-and-forget: se l'invio fallisce (socket chiuso) il WebsocketClient
        gestisce da solo la riconnessione; qui scartiamo il frame e basta.
        """
        try:
            frame = np.frombuffer(msg.data, dtype=np.uint8).reshape(
                msg.height, msg.width, 3
            )

            # Il browser si aspetta BGR (OpenCV). camera_node pubblica bgr8;
            # se la sorgente è rgb8 convertiamo prima della codifica.
            if msg.encoding == 'rgb8':
                frame = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)

            success, encoded_image = cv2.imencode(
                '.jpg', frame, [int(cv2.IMWRITE_JPEG_QUALITY), 90]
            )
            if not success:
                return

            self.stream_client.push_image(encoded_image.tobytes())
        except Exception as e:
            self.get_logger().warn(f'StreamNode encode/send error: {e}')


def main(args=None):
    rclpy.init(args=args)
    node = StreamNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.stream_client.disconnect()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
