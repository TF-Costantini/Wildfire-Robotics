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
  1. Solo in FOLLOW; persona non detect → stop
  2. distanza < min_safe_distance → blocco solo dell'AVANTI (reverse/sterzo ok)
  3. errore orizzontale ex = (cx - img_w/2) / (img_w/2) → componente angolare
  4. errore distanza ed = dist_lato_persona - target_distance → componente lineare
     (dist = sensore sul LATO dove la camera vede la persona, non min() dei due)
  5. Mix differenziale: left = v + ω, right = v - ω; saturazione ratio-preserving

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
        self.current_mode = Mode.IDLE          # B: gate sul FOLLOW
        self._last_cmd = self._stop_cmd()      # C: ultimo comando, ripubblicato a rate fisso

        # --- Subscriptions ---
        self.create_subscription(Detection, '/vision/person', self._person_callback, 10)
        self.create_subscription(Range, '/ultrasonic/left', self._ultrasonic_left_callback, 10)
        self.create_subscription(Range, '/ultrasonic/right', self._ultrasonic_right_callback, 10)
        self.create_subscription(Mode, '/mode', self._mode_callback, 10)

        # --- Publisher ---
        self._drive_pub = self.create_publisher(DriveCmd, '/cmd_drive', 10)

        # --- Control tick: ripubblica l'ultimo comando a rate FISSO (C) ---
        #    Il comando ora è disaccoppiato dal rate di detection: anche se la
        #    vision è lenta, il MCU (watchdog 1500 ms) resta alimentato e i
        #    motori non singhiozzano. Lo stesso tick fa da failsafe se la vision tace.
        period = 1.0 / self.loop_rate if self.loop_rate > 0 else 0.1
        self._control_timer = self.create_timer(period, self._control_tick)

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
        # B: fuori dal FOLLOW non comandiamo i motori (in FIRE i cingoli sono
        #    bloccati, in IDLE fermi). Una detection vagante non deve muovere il robot.
        if self.current_mode != Mode.FOLLOW:
            return

        self._last_detection_time = self.get_clock().now()
        self.person_detected = msg.found

        if msg.found:
            cmd = self._compute_drive_command(
                person_cx=msg.cx, img_width=msg.img_w,
                person_area=msg.area, img_height=msg.img_h)
        else:
            cmd = self._stop_cmd()

        # C: memorizza; la pubblicazione a rate fisso la fa _control_tick.
        #    Pubblica anche subito per reattività.
        self._last_cmd = cmd
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
        """B: traccia il mode. All'uscita dal FOLLOW azzera subito i motori."""
        prev = self.current_mode
        self.current_mode = msg.mode
        if prev == Mode.FOLLOW and msg.mode != Mode.FOLLOW:
            self._last_cmd = self._stop_cmd()
            self._drive_pub.publish(self._last_cmd)

    # ─── Control tick ─────────────────────────────────────────────────────────

    def _control_tick(self):
        """C: a rate FISSO ripubblica l'ultimo comando per tenere alimentato il
        watchdog del MCU (1500 ms) anche se la vision è lenta. Fa anche da
        failsafe: se non arrivano detection entro il timeout → STOP."""
        # Fuori dal FOLLOW non pubblichiamo nulla (B).
        if self.current_mode != Mode.FOLLOW:
            return

        now = self.get_clock().now()
        if self._last_detection_time is None:
            return

        age = (now - self._last_detection_time).nanoseconds * 1e-9
        if age > self.lost_person_timeout:
            if not self._lost:
                self.get_logger().warn(
                    f'Nessuna detection da {age:.1f}s — STOP failsafe')
                self._lost = True
            self._last_cmd = self._stop_cmd()
            self._drive_pub.publish(self._last_cmd)
        else:
            self._lost = False
            # Ripubblica l'ultimo comando valido (disaccoppia cmd-rate da detection-rate).
            self._drive_pub.publish(self._last_cmd)

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

        # 1. Steering verso il centro del bounding box (>0 ex: persona a destra).
        #    Calcolato per PRIMO: serve a scegliere il sensore lato-persona (E).
        ex = (person_cx - img_width / 2.0) / (img_width / 2.0)
        if self.invert_steer:
            ex = -ex
        if abs(ex) < self.center_deadband:
            ex = 0.0
        steer = self._clamp(ex * self.angular_kp,
                            -self.max_angular_speed, self.max_angular_speed)

        # 2. Letture ultrasoniche FRESCHE (vecchia → inf).
        left_d = self._fresh_distance(self.ultrasonic_left, self._last_left_time, now)
        right_d = self._fresh_distance(self.ultrasonic_right, self._last_right_time, now)

        # 3. Distanza LATO-PERSONA (E): non più min() dei due sensori.
        #    min() faceva indietreggiare il robot per QUALSIASI lettura vicina
        #    di UNO dei due sensori (una gamba di lato, un muro) → reverse spurio
        #    ("si avventa e poi torna indietro"). Ora usiamo il sensore allineato
        #    al lato dove la camera vede la persona; l'altro fa da fallback.
        dist = self._select_distance(ex, left_d, right_d)

        # 4. Controllo distanza su target_distance.
        if math.isfinite(dist):
            ed = dist - self.target_distance             # >0 lontano→avanti; <0 vicino→indietro
            if abs(ed) < self.distance_deadband:
                ed = 0.0                                 # dentro deadband → fermo, si attesta
            v = self._clamp(ed * self.linear_kp,
                            -self.max_linear_speed, self.max_linear_speed)
            # A: sotto la distanza di sicurezza non si spinge MAI in avanti.
            #    (reverse e sterzo restano per arretrare / restare in frame).
            if dist < self.min_safe_distance:
                v = min(v, 0.0)
        else:
            v = self.approach_speed                      # cieco = lontano → avvicinati

        # 5. Differential mix
        left = v + steer
        right = v - steer

        # 6. Cap potenza PRIMA della saturazione (G): prima il *power poteva
        #    spingere oltre 1, poi clamp/floor per-ruota schiacciavano il rapporto
        #    sterzata in curva veloce. Ora power poi saturazione ratio-preserving.
        left *= self.power_percentage
        right *= self.power_percentage

        # 7. Ratio-preserving saturation a [-1, 1]: scala ENTRAMBE le ruote,
        #    mantiene il bilancio avanti/sterzata invece di clippare ogni ruota.
        peak = max(abs(left), abs(right), 1.0)
        left /= peak
        right /= peak

        # 8. Floor di motion. Due garanzie, in ordine:
        #    (a) la ruota DOMINANTE supera il floor (scala entrambe → preserva il
        #        differenziale; fix del reverse-dritto che perdeva la persona).
        #    (b) ANTI-STALL ruota debole: se la ruota interna è alimentata ma sotto
        #        il floor (es. L=0.15, R=0.60) si pianta e FRENA la curva → il robot
        #        non gira nonostante R=0.60. La portiamo al floor (stesso segno)
        #        SOLO se la dominante è già sopra il floor, così la sterzata resta.
        pure_rotation = abs(v) < 0.05 <= abs(steer)
        if pure_rotation:
            floor = self.min_turn_speed
        else:
            floor = self.min_drive_reverse if v < 0.0 else self.min_drive

        aL, aR = abs(left), abs(right)
        peak_cmd = max(aL, aR)
        if peak_cmd > 1e-6:
            # (a) dominante ≥ floor, ratio preservato
            if peak_cmd < floor:
                scale = floor / peak_cmd
                left *= scale
                right *= scale
                aL, aR = abs(left), abs(right)
                peak_cmd = max(aL, aR)
            # (b) anti-stall ruota debole
            if peak_cmd > floor + 1e-6:
                if 1e-6 < aL < floor:
                    left = math.copysign(floor, left)
                if 1e-6 < aR < floor:
                    right = math.copysign(floor, right)

        cmd.left = round(self._clamp(left, -1.0, 1.0), 4)
        cmd.right = round(self._clamp(right, -1.0, 1.0), 4)
        return cmd

    def _select_distance(self, ex, left_d, right_d) -> float:
        """E: distanza del sensore sul LATO dove la camera vede la persona.
        ex<0 → persona a sinistra → sensore sinistro; ex>0 → destro.
        Centrata o sensore-lato cieco → fallback all'altro / minimo dei validi."""
        l_ok = math.isfinite(left_d)
        r_ok = math.isfinite(right_d)
        if ex <= -self.center_deadband:        # persona a SINISTRA
            if l_ok:
                return left_d
            if r_ok:
                return right_d
        elif ex >= self.center_deadband:       # persona a DESTRA
            if r_ok:
                return right_d
            if l_ok:
                return left_d
        # centrata o nessun lato: minimo dei validi (backstop ravvicinato)
        cands = [d for d in (left_d, right_d) if math.isfinite(d)]
        return min(cands) if cands else float('inf')


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