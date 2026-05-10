"""
camera_node.py — Wildfire Robotics UGV
Nodo ROS2 per la gestione della telecamera USB UVC.

Pubblica frame BGR su /camera/image_raw a 15 fps.
Resilient a disconnessioni temporanee della camera.
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
import os


class CameraNode(Node):
    """
    Nodo per la gestione della telecamera USB.
    Pubblica raw image su /camera/image_raw.
    """

    def __init__(self):
        super().__init__('camera_node')

        # --- Parametri ---
        self.declare_parameter('device_id', 0)      # /dev/video0
        self.declare_parameter('frame_width', 640)
        self.declare_parameter('frame_height', 480)
        self.declare_parameter('fps', 15)
        self.declare_parameter('camera_name', 'camera')

        # Modalità test: se true, usa cv2.imread invece di VideoCapture
        self.declare_parameter('test_image_path', '')  # es. '../test_fire.jpg'

        self.device_id = self.get_parameter('device_id').value
        self.frame_width = self.get_parameter('frame_width').value
        self.frame_height = self.get_parameter('frame_height').value
        self.fps = self.get_parameter('fps').value
        self.camera_name = self.get_parameter('camera_name').value
        self.test_image_path = self.get_parameter('test_image_path').value

        # --- Modalità test: immagine statica ---
        self._test_frame = None
        if self.test_image_path:
            self._test_frame = cv2.imread(self.test_image_path)
            if self._test_frame is not None:
                self.get_logger().info(f'Test mode: caricata immagine {self.test_image_path}')
            else:
                self.get_logger().error(f'Test mode: impossibile caricare {self.test_image_path}')

        # --- VideoCapture (solo se NON in modalità test) ---
        self._cap = None
        self._reconnect_attempts = 0
        if not self.test_image_path:
            self._open_camera()

        # --- CvBridge ---
        self._bridge = CvBridge()

        # --- Publisher ---
        self._pub = self.create_publisher(
            Image,
            f'/{self.camera_name}/image_raw',
            10
        )

        # --- Timer a 15 Hz ---
        self._timer = self.create_timer(1.0 / self.fps, self._timer_callback)

        log_msg = (f'CameraNode avviato: device={self.device_id}, '
                    f'{self.frame_width}x{self.frame_height} @ {self.fps} fps')
        if self.test_image_path:
            log_msg += f' [TEST IMAGE: {self.test_image_path}]'
        self.get_logger().info(log_msg)

    def _open_camera(self):
        """Apre la telecamera e configura risoluzione/fps."""
        if self._cap is not None:
            self._cap.release()
            self._cap = None

        self._cap = cv2.VideoCapture(self.device_id)

        if not self._cap.isOpened():
            self.get_logger().error(f'Impossibile aprire /dev/video{self.device_id}')
            return

        # Configura risoluzione
        self._cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.frame_width)
        self._cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.frame_height)
        # Prova a impostare fps (alcune camere non lo supportano)
        self._cap.set(cv2.CAP_PROP_FPS, self.fps)

        actual_w = int(self._cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        actual_h = int(self._cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        actual_fps = int(self._cap.get(cv2.CAP_PROP_FPS))

        self.get_logger().info(
            f'Camera aperta: {actual_w}x{actual_h} @ {actual_fps} fps'
        )

    def _timer_callback(self):
        """Acquisisce un frame (camera reale o test) e lo pubblica."""
        if self.test_image_path:
            # --- Modalità test: riproduce l'immagine statica in loop ---
            frame = self._test_frame
            if frame is None:
                return
        else:
            # --- Camera reale ---
            if self._cap is None or not self._cap.isOpened():
                self._handle_disconnection()
                return

            ret, frame = self._cap.read()

            if not ret:
                self.get_logger().warn('Frame non valido —可能的 disconnect')
                self._cap.release()
                self._cap = None
                self._handle_disconnection()
                return

        # Converti BGR → ROS2 Image
        try:
            msg = self._bridge.cv2_to_imgmsg(frame, encoding='bgr8')
        except Exception as e:
            self.get_logger().error(f'CvBridge error: {e}')
            return

        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'camera_link'

        self._pub.publish(msg)
        self._reconnect_attempts = 0  # Reset counter on success

    def _handle_disconnection(self):
        """Tenta di riaprire la camera dopo una disconnessione."""
        self._reconnect_attempts += 1

        # Every 3 seconds attempts reconnection
        if (self._reconnect_attempts % (self.fps * 3)) != 0 : return

        self.get_logger().warn(
            'Attempting Reconnection...'
        )
        self._open_camera()

    def destroy_node(self):
        """Rilascia la telecamera alla chiusura del nodo."""
        if self._cap is not None:
            self._cap.release()
            self._cap = None
            self.get_logger().info('Camera rilasciata')
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = CameraNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
