"""
follow_controller_node.py — Wildfire Robotics UGV
Nodo per il tracking di una persona con mantenimento distanza.

Input:
  - /vision/person   → Detection (persona trovata + bounding box)
  - /ultrasonic/left  → sensor_msgs/Range (distanza in metri)
  - /ultrasonic/right → sensor_msgs/Range (distanza in metri)
  - /mode            → Mode (per ignorare comandi se non in FOLLOW)

Output:
  - /cmd_drive       → DriveCmd (left/right normalizzati -1..1)

Logica:
  1. Se persona non detect → stop
  2. Se distanza < min_safe_distance → stop di sicurezza
  3. errore orizzontale ex = (cx - img_w/2) / (img_w/2) → componente angolare
  4. errore distanza ed = min(ultrasonic) - target_distance → componente lineare
  5. Mix differenziale: left = v + ω, right = v - ω, clamp [-1, 1]
"""

import rclpy
from rclpy.node import Node
from wildfire_msgs.msg import Detection, DriveCmd, Mode
from sensor_msgs.msg import Range


class FollowControllerNode(Node):

    def __init__(self):
        super().__init__('follow_controller_node')

        # --- Parametri configurabili ---
        self.declare_parameter('target_distance_cm', 80.0)
        self.declare_parameter('min_safe_distance_cm', 40.0)
        self.declare_parameter('max_distance_cm', 200.0)
        self.declare_parameter('linear_kp', 0.01)
        self.declare_parameter('angular_kp', 0.5)
        self.declare_parameter('max_linear_speed', 0.5)
        self.declare_parameter('max_angular_speed', 1.0)
        self.declare_parameter('min_turn_speed', 0.2)
        self.declare_parameter('loop_rate_hz', 10.0)

        self.target_distance = self.get_parameter('target_distance_cm').value
        self.min_safe_distance = self.get_parameter('min_safe_distance_cm').value
        self.max_distance = self.get_parameter('max_distance_cm').value
        self.linear_kp = self.get_parameter('linear_kp').value
        self.angular_kp = self.get_parameter('angular_kp').value
        self.max_linear_speed = self.get_parameter('max_linear_speed').value
        self.max_angular_speed = self.get_parameter('max_angular_speed').value
        self.min_turn_speed = self.get_parameter('min_turn_speed').value
        loop_rate = self.get_parameter('loop_rate_hz').value

        # --- Stato interno ---
        self.person_detected = False
        self.person_cx = 0.0
        self.person_cy = 0.0
        self.img_width = 640.0
        self.img_height = 480.0

        self.ultrasonic_left = float('inf')
        self.ultrasonic_right = float('inf')

        self.current_mode = Mode.IDLE

        # --- Subscriptions ---
        self.create_subscription(
            Detection,
            '/vision/person',
            self._person_callback,
            10
        )

        self.create_subscription(
            Range,
            '/ultrasonic/left',
            self._ultrasonic_left_callback,
            10
        )

        self.create_subscription(
            Range,
            '/ultrasonic/right',
            self._ultrasonic_right_callback,
            10
        )

        self.create_subscription(
            Mode,
            '/mode',
            self._mode_callback,
            10
        )

        # --- Publisher ---
        self._drive_pub = self.create_publisher(DriveCmd, '/cmd_drive', 10)

        # --- Timer di controllo (10 Hz) ---
        self.create_timer(1.0 / loop_rate, self._timer_callback)

        self.get_logger().info(
            f'FollowControllerNode avviato: target={self.target_distance} cm, '
            f'safe={self.min_safe_distance} cm'
        )

    # ─── Subscription callbacks ───────────────────────────────────────────────

    def _person_callback(self, msg: Detection):
        """Aggiorna posizione della persona rilevata."""
        self.person_detected = msg.found
        if msg.found:
            self.person_cx = msg.cx
            self.person_cy = msg.cy
            self.img_width = msg.img_w
            self.img_height = msg.img_h

    def _ultrasonic_left_callback(self, msg: Range):
        """Distanza sensore sinistro (Range è in metri)."""
        if msg.range != float('inf') and msg.range >= msg.min_range:
            self.ultrasonic_left = msg.range * 100.0  # → cm

    def _ultrasonic_right_callback(self, msg: Range):
        """Distanza sensore destro (Range è in metri)."""
        if msg.range != float('inf') and msg.range >= msg.min_range:
            self.ultrasonic_right = msg.range * 100.0  # → cm

    def _mode_callback(self, msg: Mode):
        """Tiene traccia della modalità corrente."""
        self.current_mode = msg.mode

    # ─── Timer callback ────────────────────────────────────────────────────

    def _timer_callback(self):
        """Calcola e pubblica comando drive a ogni tick."""
        # Se non siamo in FOLLOW, pubblica zero (il gating dello state_machine
        # provvederà a non inoltrarlo al MCU)
        if self.current_mode != Mode.FOLLOW:
            # TODO avoid calling this everytime. Just once after the first transition
            cmd = DriveCmd()
            cmd.left = 0.0
            cmd.right = 0.0
            cmd.stamp = self.get_clock().now().to_msg()
            self._drive_pub.publish(cmd)
            return

        cmd = self._compute_drive_command()
        self._drive_pub.publish(cmd)

    # ─── Logica di controllo ────────────────────────────────────────────────

    def _compute_drive_command(self) -> DriveCmd:
        """
        Calcola comando differenziale per inseguimento persona.

        Ritorna DriveCmd con left/right in [-1.0, 1.0].
        """
        cmd = DriveCmd()

        # 1. Nessuna persona → stop
        if not self.person_detected:
            cmd.left = 0.0
            cmd.right = 0.0
            cmd.stamp = self.get_clock().now().to_msg()
            return cmd

        # 2. Distanza minima dai due ultrasuoni
        min_dist = min(self.ultrasonic_left, self.ultrasonic_right)

        # 3. Lettura invalida (no echo o out of range) → treat as far
        if min_dist > self.max_distance or min_dist == float('inf'):
            min_dist = self.target_distance

        # 4. STOP di sicurezza se troppo vicino
        if min_dist < self.min_safe_distance:
            cmd.left = 0.0
            cmd.right = 0.0
            cmd.stamp = self.get_clock().now().to_msg()
            self.get_logger().warn(f'Sicurezza: distanza {min_dist:.1f} cm < {self.min_safe_distance} cm — STOP!')
            return cmd

        # 5. Errore orizzontale normalizzato: ex ∈ [-1, 1]
        #    ex > 0 → persona a destra → robot gira a sinistra (ω > 0)
        #    ex < 0 → persona a sinistra → robot gira a destra (ω < 0)
        ex = (self.person_cx - self.img_width / 2.0) / (self.img_width / 2.0)

        # 6. Errore distanza (cm)
        ed = min_dist - self.target_distance

        # 7. Componente lineare v (avanti/indietro)
        v = ed * self.linear_kp
        v = max(-self.max_linear_speed, min(self.max_linear_speed, v))

        # 8. Componente angolare ω (rotazione)
        omega = ex * self.angular_kp
        omega = max(-self.max_angular_speed, min(self.max_angular_speed, omega))

        # 9. Mix differenziale
        left = v + omega
        right = v - omega

        # 10. Clamp finale a [-1, 1]
        left = max(-1.0, min(1.0, left))
        right = max(-1.0, min(1.0, right))

        # 11. Rotazione pura: se quasi fermo ma deve girare, garantisce
        #     velocità angolare minima per扭头
        if abs(v) < 0.05 and abs(omega) > 0.05:
            turn_dir = 1 if omega > 0 else -1
            left = turn_dir * self.min_turn_speed
            right = -turn_dir * self.min_turn_speed

        cmd.left = round(left, 4)
        cmd.right = round(right, 4)
        cmd.stamp = self.get_clock().now().to_msg()
        return cmd


def main(args=None):
    rclpy.init(args=args)
    node = FollowControllerNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
