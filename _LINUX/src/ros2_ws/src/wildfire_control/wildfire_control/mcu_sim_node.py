"""
mcu_sim_node.py — Wildfire Robotics UGV

Simulator that impersonates the STM32U585 firmware on the ROS2 graph.
Lets the team exercise the full MPU-side stack (state_machine,
follow_controller, fire_controller, vision pipeline) without the
physical Arduino UNO Q connected.

What it mirrors from main.cpp
-----------------------------
  Publishes:
    /ultrasonic/left   sensor_msgs/Range          @ 10 Hz
    /ultrasonic/right  sensor_msgs/Range          @ 10 Hz
    /button_event      wildfire_msgs/ButtonEvent  on stdin command

  Subscribes (gated commands from state_machine_node):
    /cmd_drive_mcu     wildfire_msgs/DriveCmd
    /cmd_pantilt_mcu   wildfire_msgs/PanTiltCmd
    /cmd_laser_mcu     std_msgs/Bool

ROS parameters
--------------
  distance_left_cm   (float, default 80.0)   simulated left  range
  distance_right_cm  (float, default 80.0)   simulated right range
  publish_rate_hz    (float, default 10.0)
  fov_rad            (float, default 0.26)
  min_range_m        (float, default 0.02)
  max_range_m        (float, default 4.00)

Both distances are normal ROS parameters, so they can be changed at
runtime to script test scenarios:

    ros2 param set /mcu_sim_node distance_left_cm 50.0

Interactive console (stdin)
---------------------------
  b              emit a single PRESS on /button_event
  l <cm>         set distance_left_cm
  r <cm>         set distance_right_cm
  d <cm>         set both distances at once
  q | Ctrl+D     quit
"""

import math
import select
import sys
import threading

import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy

from sensor_msgs.msg import Range
from std_msgs.msg import Bool, Empty
from wildfire_msgs.msg import ButtonEvent, DriveCmd, PanTiltCmd


class McuSimNode(Node):

    def __init__(self):
        super().__init__('mcu_sim_node')

        # ---- Parameters --------------------------------------------------
        self.declare_parameter('distance_left_cm',  80.0)
        self.declare_parameter('distance_right_cm', 80.0)
        self.declare_parameter('publish_rate_hz',   10.0)
        self.declare_parameter('fov_rad',           0.26)
        self.declare_parameter('min_range_m',       0.02)
        self.declare_parameter('max_range_m',       4.00)

        rate_hz = float(self.get_parameter('publish_rate_hz').value)
        if rate_hz <= 0.0:
            rate_hz = 10.0

        # ---- QoS ---------------------------------------------------------
        qos = QoSProfile(
            reliability=ReliabilityPolicy.RELIABLE,
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
        )

        # ---- Publishers --------------------------------------------------
        self._pub_us_left = self.create_publisher(
            Range, '/ultrasonic/left', qos)
        self._pub_us_right = self.create_publisher(
            Range, '/ultrasonic/right', qos)
        self._pub_button = self.create_publisher(
            ButtonEvent, '/button_event', qos)

        # ---- Subscribers (gated commands from state_machine_node) -------
        self.create_subscription(
            DriveCmd, '/cmd_drive_mcu',   self._on_drive,   qos)
        self.create_subscription(
            PanTiltCmd, '/cmd_pantilt_mcu', self._on_pantilt, qos)
        self.create_subscription(
            Bool, '/cmd_laser_mcu',  self._on_laser,   qos)

        # Trigger del button via topic, utile quando il nodo viene avviato
        # da un launch file (lo stdin non e' disponibile in quel caso):
        #   ros2 topic pub --once /sim/inject_button std_msgs/msg/Empty {}
        self.create_subscription(
            Empty, '/sim/inject_button', self._on_inject_button, qos)

        # ---- Last-seen actuator state (for change-only logging) ---------
        self._last_drive   = None  # (left, right)
        self._last_pantilt = None  # (pan, tilt)
        self._last_laser   = None  # bool

        # ---- Periodic publish timer -------------------------------------
        self.create_timer(1.0 / rate_hz, self._publish_ultrasonics)

        # ---- Stdin console thread ---------------------------------------
        # Only spawn console reader when stdin is a real TTY, so the node
        # can also run from a launch file / nohup / docker -d without
        # exiting on EOF.
        self._stop = threading.Event()
        if sys.stdin and sys.stdin.isatty():
            self._stdin_thread = threading.Thread(
                target=self._stdin_loop, daemon=True)
            self._stdin_thread.start()
            self.get_logger().info(
                'McuSimNode running. Type "b" for button, "l <cm>" or '
                '"r <cm>" or "d <cm>" to set distances, "q" to quit.')
        else:
            self._stdin_thread = None
            self.get_logger().info(
                'McuSimNode running (no TTY). Use ros2 topic pub on '
                '/sim/inject_button and ros2 param set for distances.')

    # ───── Periodic publishers ────────────────────────────────────────────

    def _publish_ultrasonics(self):
        now = self.get_clock().now().to_msg()

        d_left  = float(self.get_parameter('distance_left_cm').value)  * 0.01
        d_right = float(self.get_parameter('distance_right_cm').value) * 0.01
        fov     = float(self.get_parameter('fov_rad').value)
        rmin    = float(self.get_parameter('min_range_m').value)
        rmax    = float(self.get_parameter('max_range_m').value)

        for pub, frame, value in (
            (self._pub_us_left,  'ultrasonic_left',  d_left),
            (self._pub_us_right, 'ultrasonic_right', d_right),
        ):
            msg = Range()
            msg.header.stamp = now
            msg.header.frame_id = frame
            msg.radiation_type = Range.ULTRASOUND
            msg.field_of_view  = fov
            msg.min_range      = rmin
            msg.max_range      = rmax
            # Mirror the firmware: NaN when out of bounds, real value otherwise.
            if value < rmin or value > rmax:
                msg.range = float('nan')
            else:
                msg.range = value
            pub.publish(msg)

    # ───── Subscriber callbacks ──────────────────────────────────────────

    def _on_drive(self, msg: DriveCmd):
        # Quantize to avoid spam from float jitter while still showing
        # meaningful changes (1% wheel-speed resolution).
        cur = (round(msg.left, 2), round(msg.right, 2))
        if cur != self._last_drive:
            self._last_drive = cur
            self.get_logger().info(
                f'[motor] left={cur[0]:+.2f}  right={cur[1]:+.2f}')

    def _on_pantilt(self, msg: PanTiltCmd):
        cur = (round(msg.pan_deg, 1), round(msg.tilt_deg, 1))
        if cur != self._last_pantilt:
            self._last_pantilt = cur
            self.get_logger().info(
                f'[servo] pan={cur[0]:+.1f} deg  tilt={cur[1]:+.1f} deg')

    def _on_laser(self, msg: Bool):
        if msg.data != self._last_laser:
            self._last_laser = msg.data
            self.get_logger().info(f'[laser] {"ON" if msg.data else "OFF"}')

    def _on_inject_button(self, _msg: Empty):
        self._emit_button()

    # ───── Stdin command console ─────────────────────────────────────────

    def _emit_button(self):
        msg = ButtonEvent()
        msg.stamp = self.get_clock().now().to_msg()
        msg.kind  = ButtonEvent.PRESS
        self._pub_button.publish(msg)
        self.get_logger().info('[button] PRESS emitted')

    def _set_param(self, name: str, value: float):
        self.set_parameters([rclpy.parameter.Parameter(
            name, rclpy.parameter.Parameter.Type.DOUBLE, float(value))])
        self.get_logger().info(f'[param] {name} -> {value:.1f}')

    def _handle_line(self, line: str):
        line = line.strip()
        if not line:
            return
        parts = line.split()
        cmd = parts[0].lower()

        try:
            if cmd in ('q', 'quit', 'exit'):
                self._stop.set()
                rclpy.shutdown()
            elif cmd == 'b':
                self._emit_button()
            elif cmd in ('l', 'left') and len(parts) >= 2:
                self._set_param('distance_left_cm', float(parts[1]))
            elif cmd in ('r', 'right') and len(parts) >= 2:
                self._set_param('distance_right_cm', float(parts[1]))
            elif cmd in ('d', 'dist', 'both') and len(parts) >= 2:
                v = float(parts[1])
                self._set_param('distance_left_cm',  v)
                self._set_param('distance_right_cm', v)
            else:
                self.get_logger().warn(f'unknown command: {line!r}')
        except ValueError:
            self.get_logger().warn(f'bad number in: {line!r}')

    def _stdin_loop(self):
        # select() with a timeout keeps the thread responsive to shutdown.
        while not self._stop.is_set():
            ready, _, _ = select.select([sys.stdin], [], [], 0.2)
            if not ready:
                continue
            line = sys.stdin.readline()
            if not line:               # EOF (Ctrl+D)
                self._stop.set()
                rclpy.shutdown()
                return
            self._handle_line(line)


def main(args=None):
    rclpy.init(args=args)
    node = McuSimNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node._stop.set()
        if rclpy.ok():
            node.destroy_node()
            rclpy.shutdown()


if __name__ == '__main__':
    main()
