#!/usr/bin/env python3
"""
follow_controller_node.py — Wildfire Robotics UGV
Nodo per il tracking di una persona con mantenimento distanza.

Input:
  - /vision/person    → Detection (persona trovata + bounding box)
  - /ultrasonic/left  → sensor_msgs/Range (distanza in metri)
  - /ultrasonic/right → sensor_msgs/Range (distanza in metri)
  - /mode             → Mode (per ignorare comandi se non in FOLLOW)

Output:
  - /cmd_drive       → DriveCmd (left/right normalizzati -1..1)
  - /cmd_pantilt     → PanTiltCmd (NEW: ricerca persona con pan camera)

Logica:
  1. Se persona non detect → stop ruote + RICERCA: pan oscillante
     pan_min..pan_max (SOLO PAN, niente tilt, tilt fisso a 0).
  2. Quando la persona è riacquisita a pan=θ:
       - il drive usa il BEARING totale = offset nel frame + offset camera,
         quindi il robot ruota verso la persona;
       - contemporaneamente il pan torna gradualmente a 0 (camera ri-allineata
         col fronte del robot) mentre il robot avanza verso la persona.
       - finché |bearing| > drive_align_gate_deg il robot solo ruota (gli
         ultrasuoni guardano avanti: la distanza non è quella della persona).
  3. Se distanza < min_safe_distance → stop di sicurezza.
  4. errore distanza ed = min(ultrasonic) - target_distance → componente lineare
  5. Mix differenziale: left = v + steer, right = v - steer, clamp [-1, 1]

Note (revisione):
  - Floor di marcia indietro separato (min_drive_reverse).
  - Deadband distanza ampia + velocità basse per spegnere l'hunting a 1 Hz.
  - Letture ultrasoniche con timeout.
  - Watchdog: se non arrivano detection entro lost_person_timeout_s → STOP
    failsafe; in quel caso anche la RICERCA si ferma (vision morta: inutile
    panneggiare a vuoto).
  - Cap anti-overshoot opzionale per loop lenti (est_full_speed_cm_per_s).
  - Convenzione PAN (la stessa del fire_controller): pan > 0 = camera ruotata
    a SINISTRA. Se sull'hardware risulta invertita, basta invertire il segno
    di bearing in _compute_drive_command e di search_step.
"""

import rclpy
import math
from rclpy.node import Node
from wildfire_msgs.msg import Detection, DriveCmd, Mode, PanTiltCmd
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
        self.declare_parameter('min_drive_reverse', 0.32)    # floor di marcia INDIETRO
        self.declare_parameter('loop_rate_hz', 10.0)         # usato dal watchdog
        self.declare_parameter('power_percentage', 1.0)
        self.declare_parameter('center_deadband', 0.12)      # ex deadband (normalized)
        self.declare_parameter('distance_deadband', 16.0)    # ed deadband (cm)

        # --- Sicurezza / robustezza a 1 Hz ---
        self.declare_parameter('lost_person_timeout_s', 1.5)
        self.declare_parameter('sensor_timeout_s', 1.0)
        self.declare_parameter('command_period_s', 1.0)
        self.declare_parameter('est_full_speed_cm_per_s', 0.0)

        # --- NEW: ricerca persona con pan camera (SOLO PAN, NO TILT) ---
        self.declare_parameter('pan_min_deg', -60.0)         # stessi limiti del fire ctrl
        self.declare_parameter('pan_max_deg', 60.0)
        self.declare_parameter('search_step_deg', 1.5)       # passo pan per tick
        self.declare_parameter('pan_update_hz', 10.0)        # rate timer pan
        self.declare_parameter('search_start_delay_s', 1.0)  # grace prima di cercare
        self.declare_parameter('pan_recenter_step_deg', 2.0) # rientro pan per tick
        self.declare_parameter('camera_hfov_deg', 62.0)      # FOV orizzontale camera
        self.declare_parameter('drive_align_gate_deg', 25.0) # sopra: solo rotazione

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
        self.pan_min = self.get_parameter('pan_min_deg').value
        self.pan_max = self.get_parameter('pan_max_deg').value
        self.search_step = self.get_parameter('search_step_deg').value
        self.pan_update_hz = self.get_parameter('pan_update_hz').value
        self.search_start_delay = self.get_parameter('search_start_delay_s').value
        self.pan_recenter_step = self.get_parameter('pan_recenter_step_deg').value
        self.camera_hfov = self.get_parameter('camera_hfov_deg').value
        self.drive_align_gate = self.get_parameter('drive_align_gate_deg').value

        # --- Stato ---
        self.person_detected = False
        self.ultrasonic_left = float('inf')
        self.ultrasonic_right = float('inf')
        self._last_left_time = None
        self._last_right_time = None
        self._last_detection_time = None   # ultimo messaggio (heartbeat vision)
        self._last_person_time = None      # NEW: ultimo found=True
        self._lost = False

        # --- Stato pan camera (NEW) ---
        self._current_pan = 0.0
        self._search_direction = 1         # +1 = pan crescente (camera a sinistra)
        self._searching = False
        self._last_bearing_deg = 0.0       # per partire a cercare dal lato giusto

        # --- Subscriptions ---
        self.create_subscription(Detection, '/vision/person', self._person_callback, 10)
        self.create_subscription(Range, '/ultrasonic/left', self._ultrasonic_left_callback, 10)
        self.create_subscription(Range, '/ultrasonic/right', self._ultrasonic_right_callback, 10)

        # --- Publishers ---
        self._drive_pub = self.create_publisher(DriveCmd, '/cmd_drive', 10)
        self._pantilt_pub = self.create_publisher(PanTiltCmd, '/cmd_pantilt', 10)  # NEW

        # --- Watchdog (failsafe a comando lento) ---
        period = 1.0 / self.loop_rate if self.loop_rate > 0 else 0.1
        self._watchdog_timer = self.create_timer(period, self._watchdog)

        # --- Timer pan: ricerca + rientro a home (NEW) ---
        pan_period = 1.0 / self.pan_update_hz if self.pan_update_hz > 0 else 0.1
        self._pan_timer = self.create_timer(pan_period, self._pan_update)

        # Camera dritta all'avvio
        self._publish_pantilt(0.0)

        self.get_logger().info(
            f'FollowControllerNode avviato: target={self.target_distance} cm, '
            f'safe={self.min_safe_distance} cm, rev_floor={self.min_drive_reverse}, '
            f'deadband={self.distance_deadband} cm, '
            f'search pan=[{self.pan_min}..{self.pan_max}]° (no tilt)'
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
            self._last_person_time = self._last_detection_time
            if self._searching:
                self.get_logger().info(
                    f'Persona riacquisita a pan={self._current_pan:.1f}° → '
                    f'ruoto il robot e ri-allineo la camera')
                self._searching = False
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
        # aggiungi la subscription e un guard in _person_callback e _pan_update.
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

    # ─── Pan camera: ricerca e rientro (NEW) ─────────────────────────────────

    def _pan_update(self):
        """Gestisce il pan della camera su timer:
          - persona in frame → rientro graduale del pan verso 0 (la rotazione
            del corpo è gestita dal termine di bearing nel drive, quindi la
            persona resta inquadrata mentre la camera si ri-allinea);
          - persona persa oltre il grace period → ricerca pan oscillante
            (SOLO PAN, tilt sempre 0);
          - vision muta (nessun heartbeat) → camera ferma, niente ricerca a vuoto.
        """
        now = self.get_clock().now()

        # Heartbeat vision: senza messaggi non ha senso cercare
        if self._last_detection_time is None:
            return
        vision_age = (now - self._last_detection_time).nanoseconds * 1e-9
        if vision_age > self.lost_person_timeout:
            return

        if self.person_detected:
            # FOLLOW: ri-allinea la camera col fronte del robot
            if self._current_pan != 0.0:
                if abs(self._current_pan) <= self.pan_recenter_step:
                    self._current_pan = 0.0
                else:
                    self._current_pan -= math.copysign(
                        self.pan_recenter_step, self._current_pan)
                self._publish_pantilt(self._current_pan)
            return

        # Persona non in frame: parte la ricerca dopo il grace period
        if self._last_person_time is None:
            lost_age = float('inf')
        else:
            lost_age = (now - self._last_person_time).nanoseconds * 1e-9
        if lost_age < self.search_start_delay:
            return

        if not self._searching:
            self._searching = True
            # Parti a cercare dal lato in cui la persona è stata vista l'ultima
            # volta: bearing>0 = a destra del fronte → pan deve DIMINUIRE
            # (pan>0 = camera a sinistra)
            self._search_direction = -1 if self._last_bearing_deg > 0.0 else 1
            self.get_logger().info('Persona persa → ricerca pan (no tilt)')

        next_pan = self._current_pan + self.search_step * self._search_direction
        if not (self.pan_min <= next_pan <= self.pan_max):
            self._search_direction *= -1
            next_pan = self._current_pan + self.search_step * self._search_direction
        self._current_pan = next_pan
        self._publish_pantilt(self._current_pan)

    def _publish_pantilt(self, pan: float):
        msg = PanTiltCmd()
        msg.pan_deg = float(round(pan, 2))
        msg.tilt_deg = 0.0  # richiesto: SOLO pan, niente tilt
        msg.stamp = self.get_clock().now().to_msg()
        self._pantilt_pub.publish(msg)

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

        NEW — bearing totale: l'errore angolare non è più solo l'offset nel
        frame ma il bearing rispetto al FRONTE del robot:
            bearing = offset_camera_deg - pan_deg
        (pan>0 = camera a sinistra). Così, dopo la ricerca, il robot ruota
        verso la persona mentre il pan rientra a 0; quando pan=0 il termine
        coincide col vecchio ex e tutto torna come prima.
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
        if not math.isfinite(min_dist) or min_dist > self.max_distance:
            min_dist = self.target_distance

        # 3. Safety stop
        if min_dist < self.min_safe_distance:
            cmd.left = 0.0
            cmd.right = 0.0
            self.get_logger().warn(
                f'Sicurezza: {min_dist:.1f} cm < {self.min_safe_distance} cm — STOP!')
            return cmd

        # 4. Bearing della persona rispetto al FRONTE del robot (NEW).
        #    ex_img: offset nel frame camera (>0 = a destra dell'asse camera).
        #    pan>0 = camera ruotata a SINISTRA → bearing = offset - pan.
        ex_img = (person_cx - img_width / 2.0) / (img_width / 2.0)
        half_fov = self.camera_hfov / 2.0
        bearing_deg = ex_img * half_fov - self._current_pan
        self._last_bearing_deg = bearing_deg

        # Normalizzato: con pan=0 coincide col vecchio ex (±1 = bordo immagine)
        ex = bearing_deg / half_fov
        ed = min_dist - self.target_distance

        # 4b. Persona molto fuori asse (camera ancora ruotata o persona di lato):
        #     gli ultrasuoni guardano AVANTI, quindi min_dist non è la distanza
        #     dalla persona, e spingere avanti allontanerebbe dal target.
        #     Prima ruota verso la persona, poi (sotto il gate) avanza.
        if abs(bearing_deg) > self.drive_align_gate:
            ed = 0.0

        # 5. Deadbands kill micro-jitter when already aligned / at distance
        if abs(ex) < self.center_deadband:
            ex = 0.0
        if abs(ed) < self.distance_deadband:
            ed = 0.0

        # 5b. Anti-overshoot per loop lenti (opzionale, richiede misura velocità).
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
        if self.est_full_speed > 0.0 and self.command_period > 0.0 and ed != 0.0:
            v_cap = abs(ed) / (self.est_full_speed * self.command_period)
            v = math.copysign(min(abs(v), v_cap), v)

        # 7. Differential mix
        left = v + steer
        right = v - steer

        # 8. Ratio-preserving saturation: scale BOTH wheels, don't clip each.
        peak = max(abs(left), abs(right), 1.0)
        left /= peak
        right /= peak

        # 9. Overall speed cap for this robot
        left *= self.power_percentage
        right *= self.power_percentage

        # 10. Minimum-motion guarantee — applied LAST so the output is the real command.
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