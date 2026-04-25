"""
fire_detector_node.py — Wildfire Robotics UGV
Nodo per il rilevamento fuoco tramite segmentazione colore HSV.

Input:  /camera/image_raw  (sensor_msgs/Image)
Output: /vision/fire      (Detection)

Logica:
  1. BGR → HSV
  2. Maschere rosso (H 0-10, 170-180) + arancione (H 11-25)
     con S>150, V>150 per evitare ombre/sbiaditi
  3. Morfologia open + close per pulizia
  4. findContours → blob più grande
  5. Se area > min_area → pubblica Detection con centroide
     altrimenti found=False
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from wildfire_msgs.msg import Detection
from cv_bridge import CvBridge
import cv2
import numpy as np


class FireDetectorNode(Node):
    """
    Nodo per il rilevamento fuoco basato su colori HSV.
    Configurabile per diversi ambienti e condizioni di illuminazione.
    """

    def __init__(self):
        super().__init__('fire_detector_node')

        # --- Parametri ---
        self.declare_parameter('camera_topic', '/camera/image_raw')

        # Soglie rosso (due range)
        self.declare_parameter('red_h1_low', 0)
        self.declare_parameter('red_h1_high', 10)
        self.declare_parameter('red_h2_low', 170)
        self.declare_parameter('red_h2_high', 180)

        # Soglie arancione
        self.declare_parameter('orange_h_low', 11)
        self.declare_parameter('orange_h_high', 25)

        # Soglie saturazione e value
        self.declare_parameter('sat_min', 150)
        self.declare_parameter('val_min', 150)

        # Area minima blob (px²) per essere considerato fuoco
        self.declare_parameter('min_area', 500.0)

        # Parametri morfologia
        self.declare_parameter('kernel_size', 5)

        # Flag abilitazione colori
        self.declare_parameter('enable_red', True)
        self.declare_parameter('enable_orange', True)

        camera_topic = self.get_parameter('camera_topic').value
        self.sat_min = self.get_parameter('sat_min').value
        self.val_min = self.get_parameter('val_min').value
        self.min_area = self.get_parameter('min_area').value
        self.kernel_size = self.get_parameter('kernel_size').value
        self.enable_red = self.get_parameter('enable_red').value
        self.enable_orange = self.get_parameter('enable_orange').value

        # Range H per i 3 segmenti
        self._red1 = (
            self.get_parameter('red_h1_low').value,
            self.get_parameter('red_h1_high').value
        )
        self._red2 = (
            self.get_parameter('red_h2_low').value,
            self.get_parameter('red_h2_high').value
        )
        self._orange = (
            self.get_parameter('orange_h_low').value,
            self.get_parameter('orange_h_high').value
        )

        # --- CvBridge ---
        self._bridge = CvBridge()

        # --- Kernel morfologico ---
        self._kernel = cv2.getStructuringElement(
            cv2.MORPH_ELLIPSE,
            (self.kernel_size, self.kernel_size)
        )

        # --- Subscriber ---
        self.create_subscription(
            Image,
            camera_topic,
            self._image_callback,
            10
        )

        # --- Publisher ---
        self._pub = self.create_publisher(Detection, '/vision/fire', 10)

        self.get_logger().info(
            f'FireDetectorNode avviato: sat_min={self.sat_min}, '
            f'val_min={self.val_min}, min_area={self.min_area}'
        )

    # ─── Callback principale ─────────────────────────────────────────────────

    def _image_callback(self, msg: Image):
        """Processa ogni frame e pubblica detection."""
        try:
            frame = self._bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        except Exception as e:
            self.get_logger().error(f'CvBridge error: {e}')
            return

        img_h, img_w = float(frame.shape[0]), float(frame.shape[1])
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # Costruisci maschera combinata
        mask = self._build_mask(hsv)

        # Pulizia morfologica: open (rimuove noise) + close (riempie buchi)
        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, self._kernel)
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, self._kernel)

        # Trova contorni
        contours, _ = cv2.findContours(
            mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
        )

        # Nessun blob valido → found=False
        if not contours:
            self._publish_detection(False, 0.0, 0.0, 0.0, img_w, img_h)
            return

        # Trova il blob con area massima
        largest = max(contours, key=cv2.contourArea)
        area = cv2.contourArea(largest)

        if area < self.min_area:
            self._publish_detection(False, 0.0, 0.0, 0.0, img_w, img_h)
            return

        # Calcola centroide con momenti
        M = cv2.moments(largest)
        if M['m00'] > 0:
            cx = M['m10'] / M['m00']
            cy = M['m01'] / M['m00']
        else:
            cx = float(largest[:, :, 0].mean())
            cy = float(largest[:, :, 1].mean())

        self._publish_detection(True, cx, cy, area, img_w, img_h)

    # ─── Costruzione maschera HSV ──────────────────────────────────────────

    def _build_mask(self, hsv) -> np.ndarray:
        """Costruisce la maschera binaria HSV per rosso + arancione."""
        sv_min = np.array([self.sat_min, self.sat_min, self.val_min], dtype=np.uint8)
        sv_max = np.array([255, 255, 255], dtype=np.uint8)

        masks = []

        if self.enable_red:
            # Rosso range 1: H 0-10
            lower1 = np.array([self._red1[0], self.sat_min, self.val_min], dtype=np.uint8)
            upper1 = np.array([self._red1[1], 255, 255], dtype=np.uint8)
            masks.append(cv2.inRange(hsv, lower1, upper1))

            # Rosso range 2: H 170-180
            lower2 = np.array([self._red2[0], self.sat_min, self.val_min], dtype=np.uint8)
            upper2 = np.array([self._red2[1], 255, 255], dtype=np.uint8)
            masks.append(cv2.inRange(hsv, lower2, upper2))

        if self.enable_orange:
            # Arancione: H 11-25
            lower_o = np.array([self._orange[0], self.sat_min, self.val_min], dtype=np.uint8)
            upper_o = np.array([self._orange[1], 255, 255], dtype=np.uint8)
            masks.append(cv2.inRange(hsv, lower_o, upper_o))

        if not masks:
            return np.zeros(hsv.shape[:2], dtype=np.uint8)

        # OR di tutte le maschere
        result = masks[0]
        for m in masks[1:]:
            result = cv2.bitwise_or(result, m)
        return result

    # ─── Publishing ────────────────────────────────────────────────────────

    def _publish_detection(self, found: bool, cx: float, cy: float,
                           area: float, img_w: float, img_h: float):
        """Pubblica un messaggio Detection."""
        msg = Detection()
        msg.found = found
        msg.cx = cx
        msg.cy = cy
        msg.area = area
        msg.img_w = img_w
        msg.img_h = img_h
        msg.confidence = 0.0
        msg.use_confidence = False
        self._pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = FireDetectorNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
