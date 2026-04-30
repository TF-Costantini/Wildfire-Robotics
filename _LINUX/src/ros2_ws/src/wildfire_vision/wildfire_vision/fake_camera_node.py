"""
fake_camera_node.py — Wildfire Robotics UGV
Nodo ROS2 di test che simula la telecamera del robot leggendo un file video.

Pubblica i frame su /camera/image_raw (stesso topic di camera_node) frame
by frame, in loop, così da poter testare la pipeline di vision senza
hardware fisicamente collegato.
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
import os


class FakeCameraNode(Node):
    """Nodo che pubblica frame da un file video, in sostituzione della camera reale."""

    def __init__(self):
        super().__init__('fake_camera_node')

        # --- Parametri ---
        self.declare_parameter('video_path', '')
        self.declare_parameter('frame_width', 640)
        self.declare_parameter('frame_height', 480)
        self.declare_parameter('fps', 15)
        self.declare_parameter('camera_name', 'camera')
        self.declare_parameter('loop', True)
        self.declare_parameter('use_video_fps', True)

        self.video_path = self.get_parameter('video_path').value
        self.frame_width = self.get_parameter('frame_width').value
        self.frame_height = self.get_parameter('frame_height').value
        self.fps = self.get_parameter('fps').value
        self.camera_name = self.get_parameter('camera_name').value
        self.loop = self.get_parameter('loop').value
        self.use_video_fps = self.get_parameter('use_video_fps').value

        if not self.video_path:
            self.get_logger().error('Parametro video_path non impostato — abort')
            raise RuntimeError('video_path richiesto')

        if not os.path.isfile(self.video_path):
            self.get_logger().error(f'File video non trovato: {self.video_path}')
            raise RuntimeError(f'video_path non valido: {self.video_path}')

        # --- VideoCapture su file ---
        self._cap = cv2.VideoCapture(self.video_path)
        if not self._cap.isOpened():
            self.get_logger().error(f'Impossibile aprire video: {self.video_path}')
            raise RuntimeError('VideoCapture non aperto')

        video_fps = self._cap.get(cv2.CAP_PROP_FPS)
        total_frames = int(self._cap.get(cv2.CAP_PROP_FRAME_COUNT))

        # Sceglie fps: video nativo o override
        if self.use_video_fps and video_fps and video_fps > 0:
            self.publish_fps = video_fps
        else:
            self.publish_fps = float(self.fps)

        # --- CvBridge ---
        self._bridge = CvBridge()

        # --- Publisher (stesso topic di camera_node) ---
        self._pub = self.create_publisher(
            Image,
            f'/{self.camera_name}/image_raw',
            10
        )

        # --- Timer ---
        self._timer = self.create_timer(1.0 / self.publish_fps, self._timer_callback)

        self._frame_idx = 0
        self.get_logger().info(
            f'FakeCameraNode avviato: video={self.video_path}, '
            f'native_fps={video_fps:.2f}, publish_fps={self.publish_fps:.2f}, '
            f'frames={total_frames}, loop={self.loop}'
        )

    def _timer_callback(self):
        """Legge prossimo frame dal video e lo pubblica."""
        if self._cap is None or not self._cap.isOpened():
            return

        ret, frame = self._cap.read()
        if not ret:
            # Fine del video
            if self.loop:
                self._cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
                self._frame_idx = 0
                self.get_logger().info('Video loop: restart from frame 0')
                ret, frame = self._cap.read()
                if not ret:
                    self.get_logger().error('Loop fallito — stop timer')
                    self._timer.cancel()
                    return
            else:
                self.get_logger().info('Fine video — stop timer')
                self._timer.cancel()
                return

        # Resize per matchare risoluzione attesa dalla pipeline
        if (frame.shape[1] != self.frame_width or
                frame.shape[0] != self.frame_height):
            frame = cv2.resize(frame, (self.frame_width, self.frame_height))

        try:
            msg = self._bridge.cv2_to_imgmsg(frame, encoding='bgr8')
        except Exception as e:
            self.get_logger().error(f'CvBridge error: {e}')
            return

        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'camera_link'

        self._pub.publish(msg)
        self._frame_idx += 1

    def destroy_node(self):
        if self._cap is not None:
            self._cap.release()
            self._cap = None
            self.get_logger().info('Video rilasciato')
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = FakeCameraNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()