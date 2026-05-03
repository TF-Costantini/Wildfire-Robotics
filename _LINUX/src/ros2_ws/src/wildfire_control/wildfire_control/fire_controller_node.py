"""
fire_controller_node.py — Wildfire Robotics UGV
Nodo per la ricerca e tracking del fuoco con pan-tilt e laser.

Input:
  - /vision/fire  → Detection (fuoco rilevato + bounding box)
  - /mode         → Mode (globale, per sapere se siamo in FIRE)

Output:
  - /cmd_pantilt  → PanTiltCmd
  - /cmd_laser    → std_msgs/Bool

Macchina a stati interna:
  SWEEPING → TRACKING → LOCKED

  SWEEPING: pan oscillante -60°..+60°, tilt sale a fine corsa.
            Laser OFF. Se fuoco rilevato con area>min → TRACKING.

  TRACKING: PD controller per centrare (cx,cy) al centro frame.
            Laser OFF. Se errore < soglia per 0.5s → LOCKED.
            Se fuoco perso per > 0.3s → SWEEPING.

  LOCKED:  mantiene posizione pan-tilt. Laser ON.
            Se errore > soglia o fuoco perso > 0.3s → TRACKING/SWEEPING.

Gating: se /mode non è FIRE, pubblica pantilt=(0,0) e laser=False.
"""

import rclpy
from rclpy.node import Node
from rclpy.timer import Timer
from wildfire_msgs.msg import Detection, PanTiltCmd, Mode
from std_msgs.msg import Bool
import time


class FireControllerNode(Node):

    # Stati interni
    SWEEPING = 0
    TRACKING = 1
    LOCKED = 2

    def __init__(self):
        super().__init__('fire_controller_node')

        # --- Parametri pan-tilt ---
        self.declare_parameter('pan_min_deg', -60.0)
        self.declare_parameter('pan_max_deg', 60.0)
        self.declare_parameter('tilt_min_deg', 0.0)
        self.declare_parameter('tilt_max_deg', 30.0)

        # --- Parametri sweep ---
        self.declare_parameter('sweep_step_deg', 0.2)
        self.declare_parameter('tilt_increment_deg', 0.2)
        self.declare_parameter('sweeping_update_freq', 1.0)

        # --- Parametri PD ---
        self.declare_parameter('pd_kp_pan', 0.5)
        self.declare_parameter('pd_kp_tilt', 0.5)
        self.declare_parameter('pd_kd', 0.1)

        # --- Parametri lock ---
        self.declare_parameter('lock_threshold_px', 20.0)
        self.declare_parameter('lock_time_s', 0.5)
        self.declare_parameter('unlock_time_s', 0.3)

        # --- Area minima fuoco ---
        self.declare_parameter('min_fire_area', 100.0)

        # Leggi parametri
        self.pan_min = self.get_parameter('pan_min_deg').value
        self.pan_max = self.get_parameter('pan_max_deg').value
        self.tilt_min = self.get_parameter('tilt_min_deg').value
        self.tilt_max = self.get_parameter('tilt_max_deg').value
        self.sweep_step = self.get_parameter('sweep_step_deg').value
        self.tilt_increment = self.get_parameter('tilt_increment_deg').value
        self.sweeping_update_freq = self.get_parameter('sweeping_update_freq').value
        self.pd_kp_pan = self.get_parameter('pd_kp_pan').value
        self.pd_kp_tilt = self.get_parameter('pd_kp_tilt').value
        self.pd_kd = self.get_parameter('pd_kd').value
        self.lock_threshold = self.get_parameter('lock_threshold_px').value
        self.lock_time = self.get_parameter('lock_time_s').value
        self.unlock_time = self.get_parameter('unlock_time_s').value
        self.min_fire_area = self.get_parameter('min_fire_area').value

        # --- Stato fire detection ---
        self.fire_detected = False

        # --- Stato modo globale ---
        self.current_mode = Mode.IDLE

        # --- Stato macchina interna ---
        self._internal_state = self.SWEEPING
        self._current_pan = 0.0
        self._current_tilt = self.tilt_min
        self._sweep_direction = 1  # +1 = increasing pan, -1 = decreasing
        self._tilt_direction = 1  # +1 = increasing tilt, -1 = decreasing
        self._lock_start_time = None
        self._fire_lost_time = None
        self._last_error_pan = 0.0  # per termine derivativo PD

        # --- Subscriptions ---
        self.create_subscription(
            Detection,
            '/vision/fire',
            self._fire_callback,
            10
        )

        self.create_subscription(
            Mode,
            '/mode',
            self._mode_callback,
            10
        )

        # --- Publishers ---
        self._pantilt_pub = self.create_publisher(PanTiltCmd, '/cmd_pantilt', 10)
        self._laser_pub = self.create_publisher(Bool, '/cmd_laser', 10)

        # --- Timer For Sweeping Updates ---
        self.create_timer(1.0/ self.sweeping_update_freq, self._sweeping_update)

        self.get_logger().info(
            f'FireControllerNode avviato: pan=[{self.pan_min}..{self.pan_max}]°, '
            f'tilt=[{self.tilt_min}..{self.tilt_max}]°, lock={self.lock_threshold}px/{self.lock_time}s'
        )

    # ─── Subscription callbacks ───────────────────────────────────────────────

    def _fire_callback(self, msg: Detection):
        """Aggiorna stato fuoco rilevato."""
        was_detected = self.fire_detected
        self.fire_detected = msg.found

        # Esegui stato corrente
        if self._internal_state == self.SWEEPING:
            self._run_sweeping()
        elif self._internal_state == self.TRACKING:
            self._run_tracking(time.time())
        else:
            self._run_locked(time.time())

        if self.fire_detected:
            if not was_detected:
                self._fire_lost_time = None
        else:
            # Fuoco perso: registra timestamp se non già registrato
            if was_detected and self._fire_lost_time is None:
                self._fire_lost_time = time.time()

    def _mode_callback(self, msg: Mode):
        """Transizione di stato globale: reset della macchina interna."""
        self.current_mode = msg.mode
        if self.current_mode == Mode.FIRE:
            self.get_logger().info('Entrato in FIRE → reset a SWEEPING')
            self._reset_to_sweeping()

    def _sweeping_update(self):
        if self.current_mode != Mode.FIRE or self._internal_state != self.SWEEPING: return
        self._sweeping_logic()
        self._publish_pantilt(self._current_pan, self._current_tilt)

    def _sweeping_logic(self):
        """ Runs pan and tilt sweeping logic on a timer (Only if SWEEPING enabled) """
        next_pan_angle = self._current_pan + self.sweep_step * self._sweep_direction
        # If next pan is in expected range, applies and returns
        if self.pan_min <= next_pan_angle <= self.pan_max:
            self._current_pan = next_pan_angle
            return

        # Else changes sweep direction and updates tilt
        self._sweep_direction *= -1 # Changes pan direction

        next_tilt_angle = self._current_tilt + self.tilt_increment * self._tilt_direction
        # If next tilt is in expected range, applies and returns
        if self.tilt_min <= next_tilt_angle <= self.tilt_max:
            self._current_tilt = next_tilt_angle
            return

        #Else changes tilt direction
        self._tilt_direction *= -1 # Changes tilt direction


    # ─── SWEEPING ────────────────────────────────────────────────────────────

    def _run_sweeping(self):
        # If it detects fire, transitions, actual updated to servo movements are on a background timer
        if self.fire_detected and self.fire_area >= self.min_fire_area:
            self.get_logger().info(f'Fuoco trovato! area={self.fire_area:.0f}px → TRACKING')
            self._internal_state = self.TRACKING
            self._lock_start_time = None
            self._fire_lost_time = None

    # ─── TRACKING ───────────────────────────────────────────────────────────

    def _run_tracking(self, now: float):
        """PD controller per centrare il fuoco al centro del frame."""
        # Errore: distanza del centroide dal centro immagine
        ex = self.fire_cx - self.img_width / 2.0   # px, >0 = fuoco a destra
        ey = self.fire_cy - self.img_height / 2.0  # px, >0 = fuoco in basso

        error_mag = (ex ** 2 + ey ** 2) ** 0.5

        # Check lock: errore sotto soglia per lock_time → LOCKED
        if error_mag < self.lock_threshold:
            if self._lock_start_time is None:
                self._lock_start_time = now
            elif now - self._lock_start_time >= self.lock_time:
                self.get_logger().info(f'LOCK ACQUIRED! err={error_mag:.1f}px')
                self._internal_state = self.LOCKED
                self._lock_start_time = None
        else:
            self._lock_start_time = None

        # Check fuoco perso: se manca per > unlock_time → SWEEPING
        if not self.fire_detected:
            if self._fire_lost_time is None:
                self._fire_lost_time = now
            elif now - self._fire_lost_time >= self.unlock_time:
                self.get_logger().warn('Fuoco perso > 0.3s → SWEEPING')
                self._reset_to_sweeping()
                return
        else:
            self._fire_lost_time = None

        # PD controller:
        # Segno negativo: se ex>0 (fuoco a destra) il pan deve DIMINUIRE (andare a sinistra)
        # quindi delta_pan = -kp * ex
        delta_ex = ex - self._last_error_pan
        self._last_error_pan = ex

        delta_pan = -(self.pd_kp_pan * ex + self.pd_kd * delta_ex)
        delta_tilt = -(self.pd_kp_tilt * ey)  # solo P, semplificato

        # Applica incrementi limitati (smooth)
        delta_pan = max(-5.0, min(5.0, delta_pan))
        delta_tilt = max(-5.0, min(5.0, delta_tilt))

        self._current_pan += delta_pan * 0.05
        self._current_tilt += delta_tilt * 0.05

        # Clamp ai limiti fisici
        self._current_pan = max(self.pan_min, min(self.pan_max, self._current_pan))
        self._current_tilt = max(self.tilt_min, min(self.tilt_max, self._current_tilt))

    # ─── LOCKED ─────────────────────────────────────────────────────────────

    def _run_locked(self, now: float):
        """Mantiene lock sul fuoco con correzioni minime. Laser ON."""
        if not self.fire_detected:
            if self._fire_lost_time is None:
                self._fire_lost_time = now
            elif now - self._fire_lost_time >= self.unlock_time:
                self.get_logger().warn('Lock perso → SWEEPING')
                self._reset_to_sweeping()
                return
        else:
            self._fire_lost_time = None

        # Verifica degradazione lock
        ex = self.fire_cx - self.img_width / 2.0
        ey = self.fire_cy - self.img_height / 2.0
        error_mag = (ex ** 2 + ey ** 2) ** 0.5

        if error_mag > self.lock_threshold:
            self.get_logger().info(f'Lock degradato (err={error_mag:.1f}px) → TRACKING')
            self._internal_state = self.TRACKING
            self._lock_start_time = None
            return

        # Correzioni sottili per mantenere centrato
        self._current_pan -= ex * 0.02
        self._current_tilt -= ey * 0.02

        self._current_pan = max(self.pan_min, min(self.pan_max, self._current_pan))
        self._current_tilt = max(self.tilt_min, min(self.tilt_max, self._current_tilt))

    # ─── Reset ──────────────────────────────────────────────────────────────

    def _reset_to_sweeping(self):
        """Reset completo allo stato SWEEPING."""
        self._internal_state = self.SWEEPING
        self._lock_start_time = None
        self._fire_lost_time = None
        self._last_error_pan = 0.0

    # ─── Publishing ──────────────────────────────────────────────────────────

    def _publish_pantilt(self, pan: float, tilt: float):
        msg = PanTiltCmd()
        msg.pan_deg = round(pan, 2)
        msg.tilt_deg = round(tilt, 2)
        msg.stamp = self.get_clock().now().to_msg()
        self._pantilt_pub.publish(msg)

    def _publish_laser(self, on: bool):
        msg = Bool()
        msg.data = on
        self._laser_pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = FireControllerNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()
