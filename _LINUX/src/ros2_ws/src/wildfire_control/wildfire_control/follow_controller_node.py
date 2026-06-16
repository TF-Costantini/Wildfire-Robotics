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

Note (revisione):
  - Floor di marcia indietro separato (min_drive_reverse): il reverse ha più
    attrito statico, 0.22 non basta a muovere il robot.
  - Deadband distanza più ampia + velocità più basse per spegnere l'hunting a 1 Hz.
  - Letture ultrasoniche con timeout: una lettura vecchia non resta valida per sempre.
  - Watchdog: se non arrivano detection entro lost_person_timeout_s → STOP failsafe
    (altrimenti, a 1 Hz, se la vision si blocca il robot continua con l'ultimo comando).
  - Cap anti-overshoot opzionale per loop lenti (vedi est_full_speed_cm_per_s).
"""

import rclpy
import math
from rclpy.node import Node
from wildfire_msgs.msg import Detection, DriveCmd, Mode
from sensor_msgs.msg import Range


class FollowControllerNode(Node):

    def __init__(self):
        super().__init__('follow_controller_node')

        # --- Parametri configurabili ---
        self.declare_parameter('target_distance_cm', 150.0)
        self.declare_parameter('min_safe_distance_cm', 60.0)
        self.declare_parameter('max_distance_cm', 400.0)
        self.declare_parameter('linear_kp', 0.010)
        self.declare_parameter('angular_kp', 0.45)
        self.declare_parameter('max_linear_speed', 0.25)
        self.declare_parameter('max_angular_speed', 0.45)
        self.declare_parameter('min_turn_speed', 0.26)
        self.declare_parameter('min_drive', 0.22)            # floor di marcia AVANTI
        self.declare_parameter('min_drive_reverse', 0.32)    # NEW: floor di marcia INDIETRO
        self.declare_parameter('loop_rate_hz', 10.0)         # ora usato dal watchdog
        self.declare_parameter('power_percentage', 1.0)
        self.declare_parameter('center_deadband', 0.12)      # ex deadband (normalized)
        self.declare_parameter('distance_deadband', 16.0)    # ed deadband (cm)

        # --- Sicurezza / robustezza a 1 Hz ---
        self.declare_parameter('lost_person_timeout_s', 1.5)  # NEW: stop se la vision tace
        self.declare_parameter('sensor_timeout_s', 1.0)       # NEW: scarta letture vecchie
        # Anti-overshoot per loop lenti. 0.0 = disabilitato.
        # Imposta a cm/s misurati con |cmd|=1.0 per attivarlo (vedi README in fondo).
        self.declare_parameter('command_period_s', 1.0)
        self.declare_parameter('est_full_speed_cm_per_s', 0.0)

        self.target_distance = self.get_parameter('target_distance_cm').value
        self.min_safe_distance = self.get_parameter('min_safe_distance_cm').value
        self.max_distance = self.get_parameter('max_distance_cm').value
        self.linear_kp = self.get_parameter('linear_kp').value
        self.angular_kp = self.get_parameter('angular_kp').value
        self.max_linear_speed = self.get_parameter('max_linear_speed').value
        self.max_angular_speed = self.get_parameter('max_angular_speed').value
        self.min_turn_speed = self.get_parameter('min_turn_speed').value
        self.min_drive = self.get_parameter('min_drive').value
        self.min_drive_reverse = self.get_parameter('min_drive_reverse').value
        self.power_percentage = self.get_parameter('power_percentage').value
        self.center_deadband = self.get_parameter('center_deadband').value
        self.distance_deadband = self.get_parameter('distance_deadband').value
        self.loop_rate = self.get_parameter('loop_rate_hz').value
        self.lost_person_timeout = self.get_parameter('lost_person_timeout_s').value
        self.sensor_timeout = self.get_parameter('sensor_timeout_s').value
        self.command_period = self.get_parameter('command_period_s').value
        self.est_full_speed = self.get_parameter('est_full_speed_cm_per_s').value

        # --- Stato ---
        self.person_detected = False
        self.ultrasonic_left = float('inf')
        self.ultrasonic_right = float('inf')
        self._last_left_time = None
        self._last_right_time = None
        self._last_detection_time = None
        self._lost = False

        # --- Subscriptions ---
        self.create_subscription(Detection, '/vision/person', self._person_callback, 10)
        self.create_subscription(Range, '/ultrasonic/left', self._ultrasonic_left_callback, 10)
        self.create_subscription(Range, '/ultrasonic/right', self._ultrasonic_right_callback, 10)

        # --- Publisher ---
        self._drive_pub = self.create_publisher(DriveCmd, '/cmd_drive', 10)

        # --- Watchdog (failsafe a comando lento) ---
        period = 1.0 / self.loop_rate if self.loop_rate > 0 else 0.1
        self._watchdog_timer = self.create_timer(period, self._watchdog)

        self.get_logger().info(
            f'FollowControllerNode avviato: target={self.target_distance} cm, '
            f'safe={self.min_safe_distance} cm, rev_floor={self.min_drive_reverse}, '
            f'deadband={self.distance_deadband} cm'
        )

    # ─── Subscription callbacks ───────────────────────────────────────────────

    def _person_callback(self, msg: Detection):
        """Aggiorna posizione della persona rilevata.

        Ogni messaggio (anche found=False) è un heartbeat valido della vision:
        resetta il watchdog. È l'ASSENZA di messaggi che fa scattare lo stop.
        """
        self._last_detection_time = self.get_clock().now()
        self.person_detected = msg.found

        if msg.found:
            cmd = self._compute_drive_command(person_cx=msg.cx, img_width=msg.img_w)
        else:
            cmd = self._stop_cmd()

        self._drive_pub.publish(cmd)

    def _ultrasonic_left_callback(self, msg: Range):
        """Distanza sensore sinistro (Range è in metri)."""
        if math.isfinite(msg.range) and msg.range >= msg.min_range:
            self.ultrasonic_left = msg.range * 100.0  # → cm
            self._last_left_time = self.get_clock().now()

    def _ultrasonic_right_callback(self, msg: Range):
        """Distanza sensore destro (Range è in metri)."""
        if math.isfinite(msg.range) and msg.range >= msg.min_range:
            self.ultrasonic_right = msg.range * 100.0  # → cm
            self._last_right_time = self.get_clock().now()

    def _mode_callback(self, msg: Mode):
        # NB: non ancora sottoscritto. Se vuoi rispettare il FOLLOW mode,
        # aggiungi la subscription e un guard in _person_callback.
        return

    # ─── Watchdog ─────────────────────────────────────────────────────────────

    def _watchdog(self):
        """A 1 Hz, se la vision si blocca il robot eseguirebbe l'ultimo comando
        all'infinito. Qui forziamo lo stop se non arrivano detection nel timeout."""
        now = self.get_clock().now()
        if self._last_detection_time is None:
            return
        age = (now - self._last_detection_time).nanoseconds * 1e-9
        if age > self.lost_person_timeout:
            if not self._lost:
                self.get_logger().warn(
                    f'Nessuna detection da {age:.1f}s — STOP failsafe')
                self._lost = True
                self._drive_pub.publish(self._stop_cmd())
        else:
            self._lost = False

    # ─── Logica di controllo ────────────────────────────────────────────────

    @staticmethod
    def _clamp(x, lo, hi) -> float:
        return max(lo, min(hi, x))

    @staticmethod
    def _apply_floor(w, floor_fwd, floor_rev) -> float:
        # Overcome static friction: bump a commanded wheel up to its floor, keep sign.
        # Reverse usa un floor più alto (più attrito a muovere indietro).
        if w == 0.0:
            return 0.0
        floor = floor_rev if w < 0.0 else floor_fwd
        return math.copysign(max(abs(w), floor), w)

    def _stop_cmd(self) -> DriveCmd:
        cmd = DriveCmd()
        cmd.left = 0.0
        cmd.right = 0.0
        cmd.stamp = self.get_clock().now().to_msg()
        return cmd

    def _fresh_distance(self, value, last_time, now) -> float:
        """Ritorna la lettura solo se non è più vecchia di sensor_timeout, altrimenti inf."""
        if last_time is None:
            return float('inf')
        age = (now - last_time).nanoseconds * 1e-9
        return value if age <= self.sensor_timeout else float('inf')

    def _compute_drive_command(self, person_cx, img_width) -> DriveCmd:
        """
        Differential drive command for person following.
        left/right in [-1.0, 1.0] (after power scaling).

        Mix convention (preserved from original, verify on hardware):
        a positive steer term makes the LEFT wheel faster, so the robot yaws
        toward image-right and faces a person on its right. NOTE: this is the
        opposite sign of ROS +z angular velocity — it is a steering term, not ω.
        """
        cmd = DriveCmd()
        cmd.stamp = self.get_clock().now().to_msg()

        # 0. Input validity — niente divisioni per zero / cx assurdi
        if not (math.isfinite(person_cx) and img_width and img_width > 0):
            self.get_logger().warn('Detection non valida (cx/img_w) — STOP')
            cmd.left = 0.0
            cmd.right = 0.0
            return cmd

        now = self.get_clock().now()

        # 1. Nearest obstacle — solo da letture FRESCHE (una vecchia → inf)
        left_d = self._fresh_distance(self.ultrasonic_left, self._last_left_time, now)
        right_d = self._fresh_distance(self.ultrasonic_right, self._last_right_time, now)
        min_dist = min(left_d, right_d)

        # 2. Invalid (inf/NaN) or out-of-range -> treat as "at target" (no forward push)
        #    Nessun sensore valido = non spingere avanti/indietro, ma lascia girare
        #    per restare puntato sulla persona.
        if not math.isfinite(min_dist) or min_dist > self.max_distance:
            min_dist = self.target_distance

        # 3. Safety stop
        if min_dist < self.min_safe_distance:
            cmd.left = 0.0
            cmd.right = 0.0
            self.get_logger().warn(
                f'Sicurezza: {min_dist:.1f} cm < {self.min_safe_distance} cm — STOP!')
            return cmd

        # 4. Normalized errors (>0 ex: person right; >0 ed: person too far)
        ex = (person_cx - img_width / 2.0) / (img_width / 2.0)
        ed = min_dist - self.target_distance

        # 5. Deadbands kill micro-jitter when already aligned / at distance
        if abs(ex) < self.center_deadband:
            ex = 0.0
        if abs(ed) < self.distance_deadband:
            ed = 0.0

        # 5b. Anti-overshoot per loop lenti (opzionale, richiede una misura di velocità).
        #     A 1 Hz un comando "minimo" (floor) percorre floor*vmax*period cm prima
        #     della lettura successiva. Se questo supererebbe l'errore residuo, NON
        #     muoverti: lascia che la deadband ti tenga fermo invece di oscillare.
        if self.est_full_speed > 0.0 and self.command_period > 0.0 and ed != 0.0:
            floor_dir = self.min_drive_reverse if ed < 0.0 else self.min_drive
            floor_travel = floor_dir * self.est_full_speed * self.command_period
            if abs(ed) < floor_travel:
                ed = 0.0

        # 6. P-control, each clamped to its own limit
        v = self._clamp(ed * self.linear_kp,
                        -self.max_linear_speed, self.max_linear_speed)
        steer = self._clamp(ex * self.angular_kp,
                            -self.max_angular_speed, self.max_angular_speed)

        # 6b. Cap di velocità lineare: non percorrere più dell'errore residuo in un tick.
        #     Garantisce che un singolo comando atterri AL target, non oltre.
        #     (Combinato con 5b: floor_travel <= |ed| <= v_cap*period, quindi il floor
        #     successivo non fa overshoot.)
        if self.est_full_speed > 0.0 and self.command_period > 0.0 and ed != 0.0:
            v_cap = abs(ed) / (self.est_full_speed * self.command_period)
            v = math.copysign(min(abs(v), v_cap), v)

        # 7. Differential mix
        left = v + steer
        right = v - steer

        # 8. Ratio-preserving saturation: scale BOTH wheels, don't clip each.
        #    Keeps the forward/turn balance when a wheel would exceed |1|.
        peak = max(abs(left), abs(right), 1.0)
        left /= peak
        right /= peak

        # 9. Overall speed cap for this robot
        left *= self.power_percentage
        right *= self.power_percentage

        # 10. Minimum-motion guarantee — applied LAST so the output is the real command.
        #     Pure rotation needs a higher floor (both wheels fight friction);
        #     in marcia, il floor è asimmetrico avanti/indietro.
        pure_rotation = abs(v) < 0.05 <= abs(steer)
        if pure_rotation:
            fl_fwd = fl_rev = self.min_turn_speed
        else:
            fl_fwd, fl_rev = self.min_drive, self.min_drive_reverse
        left = self._apply_floor(left, fl_fwd, fl_rev)
        right = self._apply_floor(right, fl_fwd, fl_rev)

        cmd.left = round(left, 4)
        cmd.right = round(right, 4)
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