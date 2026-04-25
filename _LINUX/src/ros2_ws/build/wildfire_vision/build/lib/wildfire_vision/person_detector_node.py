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
from wildfire_msgs.msg import Detection
from cv_bridge import CvBridge
import cv2


class PersonDetectorNode(Node):
    """
    Nodo per il rilevamento persone in tempo reale.
    Usa YOLOv8n (ultralytics), pubblica la detection più grande (più vicina).
    """

    COCO_PERSON_CLASS = 0

    def __init__(self):
        super().__init__('person_detector_node')

        # --- Parametri ---
        self.declare_parameter('camera_topic', '/camera/image_raw')
        self.declare_parameter('conf_threshold', 0.5)
        self.declare_parameter('model_name', 'yolov8n.pt')
        self.declare_parameter('device', 'cpu')

        self.camera_topic = self.get_parameter('camera_topic').value
        self.conf_threshold = self.get_parameter('conf_threshold').value
        model_name = self.get_parameter('model_name').value
        device = self.get_parameter('device').value

        # --- CvBridge ---
        self._bridge = CvBridge()

        # --- YOLOv8n ---
        self._model = None
        self._load_model(model_name, device)

        # --- Subscriber ---
        self.create_subscription(
            Image,
            self.camera_topic,
            self._image_callback,
            10
        )

        # --- Publisher ---
        self._pub = self.create_publisher(Detection, '/vision/person', 10)

        self.get_logger().info(
            f'PersonDetectorNode avviato: model={model_name}, '
            f'conf={self.conf_threshold}, device={device}'
        )

    # ─── Model loading ───────────────────────────────────────────────────────

    def _load_model(self, model_name: str, device: str):
        """Carica YOLOv8n con fallback in caso di errore."""
        try:
            from ultralytics import YOLO
            self._model = YOLO(model_name)
            self.get_logger().info(f'YOLOv8n caricato: {model_name}')
        except Exception as e:
            self.get_logger().error(f'Impossibile caricare YOLOv8n: {e}')
            self.get_logger().warn('YOLOv8n non disponibile — il nodo non pubblicherà detection')
            self._model = None

    # ─── Callback principale ─────────────────────────────────────────────────

    def _image_callback(self, msg: Image):
        """Processa ogni frame e pubblica detection persona."""
        # Converte ROS → OpenCV
        try:
            frame = self._bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        except Exception as e:
            self.get_logger().error(f'CvBridge error: {e}')
            return

        img_h, img_w = frame.shape[:2]

        # Nessun modello → pubblica found=False
        if self._model is None:
            self._publish_detection(False, 0.0, 0.0, 0.0, img_w, img_h, 0.0)
            return

        # Inference YOLOv8n
        try:
            results = self._model(frame, verbose=False, device=device)
        except Exception as e:
            self.get_logger().error(f'YOLO inference error: {e}')
            self._publish_detection(False, 0.0, 0.0, 0.0, img_w, img_h, 0.0)
            return

        boxes = results[0].boxes
        if boxes is None or len(boxes) == 0:
            self._publish_detection(False, 0.0, 0.0, 0.0, img_w, img_h, 0.0)
            return

        # Filtra: classe person (0) + confidence minima
        person_boxes = []
        for box in boxes:
            cls = int(box.cls[0].item())
            conf = float(box.conf[0].item())
            if cls == self.COCO_PERSON_CLASS and conf >= self.conf_threshold:
                person_boxes.append(box)

        if not person_boxes:
            self._publish_detection(False, 0.0, 0.0, 0.0, img_w, img_h, 0.0)
            return

        # Seleziona la bbox con area maggiore (persona più vicina)
        largest = max(person_boxes, key=lambda b: float(b.conf[0]))

        # Calcola centroide e area della bbox
        x1, y1, x2, y2 = largest.xyxy[0].cpu().numpy()
        cx = float((x1 + x2) / 2.0)
        cy = float((y1 + y2) / 2.0)
        area = float((x2 - x1) * (y2 - y1))
        conf = float(largest.conf[0].item())

        self._publish_detection(True, cx, cy, area, img_w, img_h, conf)

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
