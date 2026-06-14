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
            callback=self._inference_callback # This runs when data comes back
        )


    def _start_camera_processing(self):
        if self.camera_subscription is not None:
            return

        self.yolo_client.connect()

        self.camera_subscription = self.create_subscription(
            Image,
            self.camera_topic,
            self.send_to_inference_server,
            10
        )

    def _stop_camera_processing(self):
        if self.camera_subscription is None:
            return

        self.yolo_client.disconnect()
        self.destroy_subscription(self.camera_subscription)
        self.camera_subscription = None

    # ─── Callback principale ─────────────────────────────────────────────────

    async def send_to_inference_server(self, msg: Image):
        """Send one image file and return the parsed response."""
        image_bytes = convert_ros_msg_to_bytes(msg)
        self.yolo_client.push_image(image_bytes)

    def _mode_callback(self, mode_msg: Mode):
        if mode_msg.mode == Mode.FOLLOW:
            self._start_camera_processing()
            return
        self._stop_camera_processing()

    def _inference_callback(self, result: dict):
        status = result["status"]

        if status != "ok":
            self.get_logger().info(f"ERROR while processing YOLO Inference...")
            return

        self.get_logger().info(f"New YOLO Results Received...")
        detection = result["detection"]
        if detection is None:
            self._publish_detection(False, 0.0, 0.0, 0.0, 0.0, 0.0, 1)
            return

        self._publish_detection(
            True,
            result["cx"],
            result["cy"],
            result["area"],
            result["img_w"],
            result["img_h"],
            result["conf"]
        )

    # ─── Publishing ─────────────────────────────────────────────────────────

    def _publish_detection(self, found: bool, cx: float, cy: float,
                          area: float, img_w: float, img_h: float,
                          confidence: float):
        """Pubblica un messaggio Detection."""
        msg = Detection()
        msg.found = found
        msg.cx = cx
        msg.cy = cy
        msg.area = area
        msg.img_w = img_w
        msg.img_h = img_h
        msg.confidence = confidence
        msg.use_confidence = True
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
