"""
state_machine_node.py — Wildfire Robotics UGV
Nodo centrale per la gestione della macchina a stati.

Responsabilità:
- Gestisce la rotazione IDLE → FOLLOW → FIRE → IDLE via button_event
- Implementa il gating centrale: solo il controller attivo può pilotare gli attuatori
- Watchdog 500ms: pubblica comando neutro se non arrivano comandi
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

        # Watchdog
        self.declare_parameter('watchdog_timeout_ms', 500)
        self.watchdog_timeout_ms = self.get_parameter('watchdog_timeout_ms').value

        # Pan-tilt home position
        self.declare_parameter('pan_home', 0.0)
        self.declare_parameter('tilt_home', 0.0)
        self.pan_home = self.get_parameter('pan_home').value
        self.tilt_home = self.get_parameter('tilt_home').value

        # Lock per thread safety
        self.lock = threading.Lock()

        # Timestamp ultimo comando ricevuto (in secondi, per confronto semplice)
        self._last_cmd_sec = self.get_clock().now().nanoseconds / 1e9

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

        # cmd_drive dal follow_controller
        self.create_subscription(
            DriveCmd,
            '/cmd_drive',
            self._drive_callback,
            qos
        )

        # cmd_pantilt dal fire_controller
        self.create_subscription(
            PanTiltCmd,
            '/cmd_pantilt',
            self._pantilt_callback,
            qos
        )

        # cmd_laser dal fire_controller
        self.create_subscription(
            Bool,
            '/cmd_laser',
            self._laser_callback,
            qos
        )

        # === PUBLICATIONS ===

        # Stato corrente (latched)
        self._mode_pub = self.create_publisher(Mode, '/mode', qos_latched)

        # Comandi gated verso MCU
        self._drive_pub = self.create_publisher(DriveCmd, '/cmd_drive_mcu', qos)
        self._pantilt_pub = self.create_publisher(PanTiltCmd, '/cmd_pantilt_mcu', qos)
        self._laser_pub = self.create_publisher(Bool, '/cmd_laser_mcu', qos)

        # === Valori ultimi ricevuti dai controller ===
        self._drive_cmd = DriveCmd()
        self._drive_cmd.left = 0.0
        self._drive_cmd.right = 0.0

        self._pantilt_cmd = PanTiltCmd()
        self._pantilt_cmd.pan_deg = self.pan_home
        self._pantilt_cmd.tilt_deg = self.tilt_home

        self._laser_cmd = Bool()
        self._laser_cmd.data = False

        # === Timer watchdog (10 Hz → check ogni 100 ms) ===
        self.create_timer(0.1, self._watchdog_callback)

        # Pubblicare stato iniziale e comandi iniziali
        self._publish_mode()
        self._apply_gating()

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

            self.get_logger().info(f'STATE: {state_names[old_state]} → {state_names[self.current_state]}')

        self._publish_mode()
        # Azzerare comandi al cambio di stato prima del prossimo gating
        self._reset_to_neutral()
        self._apply_gating()

    def _drive_callback(self, msg: DriveCmd):
        """Memorizza l'ultimo cmd_drive ricevuto e aggiorna watchdog."""
        with self.lock:
            self._drive_cmd = msg
            self._last_cmd_sec = self.get_clock().now().nanoseconds / 1e9

    def _pantilt_callback(self, msg: PanTiltCmd):
        """Memorizza l'ultimo cmd_pantilt ricevuto e aggiorna watchdog."""
        with self.lock:
            self._pantilt_cmd = msg
            self._last_cmd_sec = self.get_clock().now().nanoseconds / 1e9

    def _laser_callback(self, msg: Bool):
        """Memorizza l'ultimo cmd_laser ricevuto e aggiorna watchdog."""
        with self.lock:
            self._laser_cmd = msg
            self._last_cmd_sec = self.get_clock().now().nanoseconds / 1e9

    # ─── Watchdog ────────────────────────────────────────────────────────────

    def _watchdog_callback(self):
        """Se non arrivano comandi da 500 ms, pubblica comando neutro."""
        elapsed = (self.get_clock().now().nanoseconds / 1e9) - self._last_cmd_sec
        if elapsed > (self.watchdog_timeout_ms / 1000.0):
            self.get_logger().warn(f'Watchdog timeout ({self.watchdog_timeout_ms} ms senza comandi) — pubblico neutro')
            self._reset_to_neutral()
            self._apply_gating()
            # Reset del timestamp per non ripetere il warning ogni tick
            self._last_cmd_sec = self.get_clock().now().nanoseconds / 1e9

    # ─── Publishing ─────────────────────────────────────────────────────────

    def _publish_mode(self):
        """Pubblica lo stato corrente su /mode (latched)."""
        msg = Mode()
        msg.mode = self.current_state
        msg.stamp = self.get_clock().now().to_msg()
        self._mode_pub.publish(msg)

    def _reset_to_neutral(self):
        """Azzera tutti i comandi allo stato neutro."""
        self._drive_cmd.left = 0.0
        self._drive_cmd.right = 0.0
        self._pantilt_cmd.pan_deg = self.pan_home
        self._pantilt_cmd.tilt_deg = self.tilt_home
        self._laser_cmd.data = False

    def _apply_gating(self):
        """
        Gating centrale — mutua esclusione FOLLOW / FIRE.

        IDLE  → tutto ZERO (motori, pantilt, laser)
        FOLLOW → solo drive attivo; pantilt→home; laser OFF
        FIRE  → solo pantilt+laser attivi; drive ZERO
        """
        with self.lock:
            if self.current_state == self.STATE_IDLE:
                # Tutto spento
                self._publish_drive(0.0, 0.0)
                self._publish_pantilt(self.pan_home, self.tilt_home)
                self._publish_laser(False)

            elif self.current_state == self.STATE_FOLLOW:
                # Solo drive; pantilt a home; laser OFF
                self._publish_drive(self._drive_cmd.left, self._drive_cmd.right)
                self._publish_pantilt(self.pan_home, self.tilt_home)
                self._publish_laser(False)

            else:  # FIRE
                # Solo pantilt e laser; drive ZERO
                self._publish_drive(0.0, 0.0)
                self._publish_pantilt(self._pantilt_cmd.pan_deg, self._pantilt_cmd.tilt_deg)
                self._publish_laser(self._laser_cmd.data)

    def _publish_drive(self, left: float, right: float):
        msg = DriveCmd()
        msg.left = left
        msg.right = right
        msg.stamp = self.get_clock().now().to_msg()
        self._drive_pub.publish(msg)

    def _publish_pantilt(self, pan: float, tilt: float):
        msg = PanTiltCmd()
        msg.pan_deg = pan
        msg.tilt_deg = tilt
        msg.stamp = self.get_clock().now().to_msg()
        self._pantilt_pub.publish(msg)

    def _publish_laser(self, on: bool):
        msg = Bool()
        msg.data = on
        self._laser_pub.publish(msg)


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
