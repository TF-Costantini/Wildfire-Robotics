from arduino.app_utils import *
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Range
from std_msgs.msg import Int32
from wildfire_msgs.msg import ButtonEvent
from arduino.app_utils import Bridge, App
import threading

def get_label(kind: int):
    match kind:
        case 0:
            return ButtonEvent.PRESS
        case 2:
            return ButtonEvent.LONG_PRESS
        case _:
            return ButtonEvent.RELEASE


class UnoQBridgeNode(Node):
    def __init__(self):
        super().__init__('comm_bridge_node')

        self._pantilt_sub = self.create_subscription(
            PanTiltCmd,
            '/cmd_pantilt',
            self._on_pantilt,
            10
        )

        self._laser_sub = self.create_subscription(
            Bool,
            '/cmd_laser',
            self._on_laser,
            10
        )

        self._drive_sub = self.create_subscription(
            DriveCmd,
            '/cmd_drive',
            self._on_drive,
            10
        )

        self.ultrasonic_left_pub  = self.create_publisher(Range, '/ultrasonic/left', 10)
        self.ultrasonic_right_pub = self.create_publisher(Range, '/ultrasonic/right', 10)
        self.button_pub           = self.create_publisher(ButtonEvent, '/button_event', 10)

        Bridge.provide("on_ultrasonic", self.on_ultrasonic)
        Bridge.provide("on_button", self.on_button)

        self.get_logger().info("UNO Q Bridge node started")

    def _on_pantilt(self, msg: PanTiltCmd):
        Bridge.call("set_pantilt", msg.pan_deg, msg.tilt_deg)
        return

    def _on_laser(self, msg: Bool):
        Bridge.call("set_laser", msg.data)
        return
    def _on_drive(self, msg):
        Bridge.call("set_drive", msg.left, msg.right)
        return

    def on_ultrasonic(self, left_m: float, right_m: float):
        stamp = self.get_clock().now().to_msg()

        for pub, distance in ((self.ultrasonic_left_pub, left_m),
                              (self.ultrasonic_right_pub, right_m)):
            msg = Range()
            msg.header.stamp    = stamp
            msg.header.frame_id = "ultrasonic"
            msg.radiation_type  = Range.ULTRASOUND
            msg.min_range       = 0.02
            msg.max_range       = 4.0
            msg.range           = distance
            pub.publish(msg)

    def on_button(self, kind: int):
        self._handle_button(kind)

    def _handle_button(self, kind: int):
        self.get_logger().info("Received Button From MPU")

        msg = ButtonEvent()
        msg.stamp = self.get_clock().now().to_msg()
        msg.kind = get_label(kind)
        self.button_pub.publish(msg)


def main():
    rclpy.init()
    node = UnoQBridgeNode()

    # Spin ROS in a background thread — Bridge.provide callbacks run on the Bridge thread
    ros_thread = threading.Thread(target=rclpy.spin, args=(node,), daemon=True)
    ros_thread.start()

    App.run()  # blocks, keeps Bridge alive

    rclpy.shutdown()


if __name__ == '__main__':
    main()