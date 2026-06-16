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
        self.declare_parameter('approach_speed', 0.25)       # avanti costante quando ULTRASUONI CIECHI (persona lontana)
        self.declare_parameter('loop_rate_hz', 10.0)         # ora usato dal watchdog
        self.declare_parameter('power_percentage', 1.0)
        self.declare_parameter('center_deadband', 0.12)      # ex deadband (normalized)
        self.declare_parameter('distance_deadband', 16.0)    # ed deadband (cm)
        # Se la camera è montata specchiata (o i cingoli sono cablati invertiti),
        # il robot gira dalla parte SBAGLIATA: persona a destra → muso a sinistra.
        # Metti true per invertire il segno dello sterzo senza ricablare. TARARE A MANO.
        self.declare_parameter('invert_steer', False)

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
        self.approach_speed = self.get_parameter('approach_speed').value
        self.power_percentage = self.get_parameter('power_percentage').value
        self.center_deadband = self.get_parameter('center_deadband').value
        self.distance_deadband = self.get_parameter('distance_deadband').value
        self.invert_steer = self.get_parameter('invert_steer').value
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
            cmd = self._compute_drive_command(
                person_cx=msg.cx, img_width=msg.img_w,
                person_area=msg.area, img_height=msg.img_h)
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

    def _compute_drive_command(self, person_cx, img_width,
                               person_area=0.0, img_height=0.0) -> DriveCmd:
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

        # 1. Letture ultrasoniche FRESCHE (vecchia → inf). Usate solo come
        #    backstop ravvicinato: da lontano non leggono.
        left_d = self._fresh_distance(self.ultrasonic_left, self._last_left_time, now)
        right_d = self._fresh_distance(self.ultrasonic_right, self._last_right_time, now)

        # 2. Controllo distanza su target_distance (ULTRASUONI, OR dei due sensori).
        #    dist = la più VICINA tra le letture valide → se uno dei due vede < target
        #    si indietreggia. Nessuna lettura valida = persona oltre il range US ma
        #    vista dalla camera → AVANTI costante finché un sensore aggancia.
        candidates = [d for d in (left_d, right_d) if math.isfinite(d)]
        if candidates:
            dist = min(candidates)                       # OR: il più vicino comanda
            ed = dist - self.target_distance             # >0 lontano→avanti; <0 vicino→indietro
            if abs(ed) < self.distance_deadband:
                ed = 0.0                                 # dentro deadband → fermo, si attesta
            v = self._clamp(ed * self.linear_kp,
                            -self.max_linear_speed, self.max_linear_speed)
        else:
            v = self.approach_speed                      # cieco = lontano → avvicinati

        # 3. Steering verso il centro del bounding box (>0 ex: persona a destra).
        #    Attivo sempre: traccia e gira mentre segue.
        ex = (person_cx - img_width / 2.0) / (img_width / 2.0)
        if self.invert_steer:
            ex = -ex
        if abs(ex) < self.center_deadband:
            ex = 0.0
        steer = self._clamp(ex * self.angular_kp,
                            -self.max_angular_speed, self.max_angular_speed)

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
        #     BUGFIX: il vecchio _apply_floor() alzava OGNI ruota al suo floor in
        #     modo indipendente. In retromarcia / vicino allo stop entrambe le
        #     ruote venivano portate allo STESSO floor → differenziale annullato →
        #     il robot smetteva di girare e indietreggiava dritto, perdendo la
        #     persona dal frame (il sintomo riportato). Ora scaliamo ENTRAMBE le
        #     ruote dello stesso fattore: la ruota dominante raggiunge il floor e
        #     il differenziale (cioè la sterzata) è preservato.
        pure_rotation = abs(v) < 0.05 <= abs(steer)
        if pure_rotation:
            floor = self.min_turn_speed
        else:
            floor = self.min_drive_reverse if v < 0.0 else self.min_drive

        peak_cmd = max(abs(left), abs(right))
        if 1e-6 < peak_cmd < floor:
            scale = floor / peak_cmd
            left *= scale
            right *= scale

        cmd.left = round(self._clamp(left, -1.0, 1.0), 4)
        cmd.right = round(self._clamp(right, -1.0, 1.0), 4)
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