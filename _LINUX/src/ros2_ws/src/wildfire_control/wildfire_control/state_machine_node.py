"""
state_machine_node.py — Wildfire Robotics UGV
Nodo per la gestione della macchina a stati.
"""

import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy, DurabilityPolicy
from wildfire_msgs.msg import Mode, ButtonEvent, DriveCmd, PanTiltCmd
from std_msgs.msg import Bool
import threading


class StateMachineNode(Node):

    def __init__(self):
        super().__init__('state_machine_node')

        # Costanti di stato (allineate a Mode.msg)
        self.STATE_IDLE = Mode.IDLE    # 0
        self.STATE_FOLLOW = Mode.FOLLOW  # 1
        self.STATE_FIRE = Mode.FIRE    # 2

        # Stato corrente — parte da IDLE
        self.current_state = self.STATE_IDLE

        # Lock per thread safety
        self.lock = threading.Lock()

        # QoS standard
        qos = QoSProfile(
            reliability=ReliabilityPolicy.RELIABLE,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )

        # QoS latched per /mode (i nuovi subscriber ricevono l'ultimo valore subito)
        qos_latched = QoSProfile(
            reliability=ReliabilityPolicy.RELIABLE,
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
            durability=DurabilityPolicy.TRANSIENT_LOCAL
        )

        # === SUBSCRIPTIONS ===

        # Button event → transizioni di stato
        self.create_subscription(
            ButtonEvent,
            '/button_event',
            self._button_callback,
            qos
        )

        # === PUBLICATIONS ===

        # Stato corrente (latched)
        self._mode_pub = self.create_publisher(Mode, '/mode', qos_latched)

        # Pubblicare stato iniziale
        self._publish_mode()

        state_names = {self.STATE_IDLE: 'IDLE', self.STATE_FOLLOW: 'FOLLOW', self.STATE_FIRE: 'FIRE'}
        self.get_logger().info(f'StateMachineNode avviato in {state_names[self.current_state]}')

    # ─── Subscription callbacks ───────────────────────────────────────────────

    def _button_callback(self, msg: ButtonEvent):
        """Ruota gli stati IDLE → FOLLOW → FIRE → IDLE."""
        if msg.kind != ButtonEvent.PRESS:
            return

        state_names = {self.STATE_IDLE: 'IDLE', self.STATE_FOLLOW: 'FOLLOW', self.STATE_FIRE: 'FIRE'}

        with self.lock:
            old_state = self.current_state
            if self.current_state == self.STATE_IDLE:
                self.current_state = self.STATE_FOLLOW
            elif self.current_state == self.STATE_FOLLOW:
                self.current_state = self.STATE_FIRE
            else:  # FIRE
                self.current_state = self.STATE_IDLE

            self._publish_mode()
            self.get_logger().info(f'STATE: {state_names[old_state]} → {state_names[self.current_state]}')

    def _publish_mode(self):
        """Pubblica lo stato corrente su /mode (latched)."""
        msg = Mode()
        msg.mode = self.current_state
        msg.stamp = self.get_clock().now().to_msg()
        self._mode_pub.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = StateMachineNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
