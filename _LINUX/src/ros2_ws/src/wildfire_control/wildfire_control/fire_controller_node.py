#!/usr/bin/env python3
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
  SWEEPING → TRACKING → LOCKED   (come prima, ma con un TARGET SELECTOR a monte)

  SWEEPING: pan oscillante pan_min..pan_max, tilt sale a fine corsa.
            Laser OFF. Se target valido (area>min) → TRACKING.
  TRACKING: PD controller per centrare il TARGET FILTRATO al centro frame.
            Laser OFF. Se errore < soglia per lock_time → LOCKED.
            Se target scaduto (unlock_time) → SWEEPING.
  LOCKED:   correzioni minime sul target filtrato. Laser ON.
            Se errore > soglia → TRACKING. Se target scaduto → SWEEPING.

Revisione multi-patch (questa versione):
  - Problema: con più patch di colore nel frame, le detection "saltano" da una
    patch all'altra a ogni messaggio e il PD inseguiva un bersaglio diverso
    ogni volta (la macchina a stati impazziva).
  - Soluzione: livello di selezione/associazione che mantiene UN solo target,
    la patch PIÙ GRANDE:
      * gate di associazione (assoc_gate_px): le detection vicine al target
        corrente lo aggiornano (è la stessa patch che si muove);
      * detection lontane → IGNORATE, a meno che non siano nettamente più
        grandi del target corrente (switch_area_ratio): in quel caso si
        aggancia la patch maggiore (con drop a TRACKING se eravamo LOCKED,
        perché il nuovo centro non è centrato);
      * centroide e area filtrati con EMA (ema_alpha) per togliere il jitter.
  - Il PD in TRACKING/LOCKED lavora sempre sul target filtrato, mai sul
    messaggio raw.
  - Laser: ON solo all'ingresso in LOCKED, OFF a ogni uscita. Lock sul centro
    della patch più grande → laser engaged.
  - Homing senza threading.Timer: tutto gestito dal timer ROS (flag _homing).
    Prima homing e sweep giravano in parallelo e si litigavano il pan.
  - Failsafe nel timer: se la vision smette di pubblicare mentre siamo in
    TRACKING/LOCKED, il target scade → reset a SWEEPING (laser OFF).

Gating: se /mode non è FIRE, laser OFF e pan-tilt riportato a home.
"""

import math
import time

import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from wildfire_msgs.msg import Detection, PanTiltCmd, Mode
from std_msgs.msg import Bool


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

        # --- Parametri target selector (multi-patch) NEW ---
        # Raggio entro cui una detection è considerata "la stessa patch"
        self.declare_parameter('assoc_gate_px', 120.0)
        # Una patch DIVERSA ruba il target solo se area > ratio * area corrente
        self.declare_parameter('switch_area_ratio', 1.4)
        # Filtro esponenziale su centroide/area (1.0 = nessun filtro)
        self.declare_parameter('ema_alpha', 0.35)
        # Failsafe timer: se nessuna detection valida da così tanto in
        # TRACKING/LOCKED → reset (copre anche vision morta)
        self.declare_parameter('vision_timeout_s', 1.0)
        # False = ignora /mode (test a banco senza supervisore che pubblica Mode.FIRE)
        self.declare_parameter('require_fire_mode', True)

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
        self.assoc_gate = self.get_parameter('assoc_gate_px').value
        self.switch_area_ratio = self.get_parameter('switch_area_ratio').value
        self.ema_alpha = self.get_parameter('ema_alpha').value
        self.vision_timeout = self.get_parameter('vision_timeout_s').value
        self.require_fire_mode = self.get_parameter('require_fire_mode').value

        # --- Stato modo globale ---
        self.current_mode = Mode.IDLE

        # --- Stato macchina interna ---
        self._internal_state = self.SWEEPING
        self._current_pan = 0.0
        self._current_tilt = self.tilt_min
        self._sweep_direction = 1   # +1 = pan crescente, -1 = decrescente
        self._tilt_direction = 1    # +1 = tilt crescente, -1 = decrescente
        self._homing = False        # NEW: homing gestito dal timer ROS
        self._lock_start_time = None
        self._last_error_pan = 0.0  # per termine derivativo PD
        self._laser_on = False

        # --- Stato target selezionato (filtrato) NEW ---
        self._tgt_cx = 0.0
        self._tgt_cy = 0.0
        self._tgt_area = 0.0
        self._tgt_last_seen = None  # time.time() dell'ultimo update associato
        self._img_w = 0.0
        self._img_h = 0.0

        # --- Subscriptions ---
        self.create_subscription(Detection, '/vision/fire', self._fire_callback, qos_profile_sensor_data)  # BEST_EFFORT: compatibile con publisher sensor-data
        self.create_subscription(Mode, '/mode', self._mode_callback, 10)

        # --- Publishers ---
        self._pantilt_pub = self.create_publisher(PanTiltCmd, '/cmd_pantilt', 10)
        self._laser_pub = self.create_publisher(Bool, '/cmd_laser', 10)

        # --- Timer: sweep + homing + failsafe ---
        self.create_timer(1.0 / self.sweeping_update_freq, self._timer_update)

        self.get_logger().info(
            f'FireControllerNode avviato: pan=[{self.pan_min}..{self.pan_max}]°, '
            f'tilt=[{self.tilt_min}..{self.tilt_max}]°, lock={self.lock_threshold}px/{self.lock_time}s, '
            f'gate={self.assoc_gate}px, switch_ratio={self.switch_area_ratio}'
        )

    # ─── Target selector (multi-patch) ───────────────────────────────────────

    def _target_valid(self, now: float) -> bool:
        """Il target è valido se aggiornato entro unlock_time."""
        return (self._tgt_last_seen is not None
                and (now - self._tgt_last_seen) <= self.unlock_time)

    def _update_target(self, msg: Detection, now: float):
        """Associa la detection al target corrente o decide se cambiarlo.

        Politica: si tiene SEMPRE la patch più grande.
          - Nessun target attivo → adotta la detection.
          - Detection entro assoc_gate_px dal target → stessa patch → EMA update.
          - Detection lontana → altra patch: la si adotta SOLO se la sua area
            supera switch_area_ratio * area corrente, altrimenti la si ignora.
        """
        # Nessun target attivo (mai visto o scaduto) → adotta
        if (self._tgt_last_seen is None
                or (now - self._tgt_last_seen) > self.unlock_time):
            self._tgt_cx = float(msg.cx)
            self._tgt_cy = float(msg.cy)
            self._tgt_area = float(msg.area)
            self._tgt_last_seen = now
            return

        dist = math.hypot(msg.cx - self._tgt_cx, msg.cy - self._tgt_cy)

        if dist <= self.assoc_gate:
            # Stessa patch → smoothing EMA su centroide e area
            a = self.ema_alpha
            self._tgt_cx += a * (msg.cx - self._tgt_cx)
            self._tgt_cy += a * (msg.cy - self._tgt_cy)
            self._tgt_area += a * (msg.area - self._tgt_area)
            self._tgt_last_seen = now
            return

        # Patch diversa: ruba il target solo se nettamente più grande
        if msg.area > self.switch_area_ratio * self._tgt_area:
            self.get_logger().info(
                f'Switch target: patch più grande ({msg.area:.0f}px vs '
                f'{self._tgt_area:.0f}px, dist={dist:.0f}px)')
            self._tgt_cx = float(msg.cx)
            self._tgt_cy = float(msg.cy)
            self._tgt_area = float(msg.area)
            self._tgt_last_seen = now
            self._lock_start_time = None
            self._last_error_pan = 0.0
            if self._internal_state == self.LOCKED:
                # Il nuovo target non è centrato → torna a TRACKING, laser OFF
                self.get_logger().info('Nuovo target non centrato → TRACKING')
                self._internal_state = self.TRACKING
                self._publish_laser(False)
        # else: patch minore e lontana → ignorata, il target resta quello

    # ─── Subscription callbacks ───────────────────────────────────────────────

    def _fire_callback(self, msg: Detection):
        if self.require_fire_mode and self.current_mode != Mode.FIRE:
            # NON silenzioso: se nessuno pubblica /mode con Mode.FIRE il nodo
            # non farà MAI nulla. Per test a banco: require_fire_mode:=false
            self.get_logger().warn(
                'Detection scartata: mode != FIRE. Pubblica Mode.FIRE su /mode '
                'oppure lancia con -p require_fire_mode:=false',
                throttle_duration_sec=5.0)
            return

        now = time.time()

        # Aggiorna il target selezionato (solo detection valide)
        if msg.found:
            self._img_w = float(msg.img_w)
            self._img_h = float(msg.img_h)
            if msg.area >= self.min_fire_area:
                self._update_target(msg, now)

        # Esegui stato corrente sul TARGET FILTRATO
        if self._internal_state == self.SWEEPING:
            if self._target_valid(now):
                self.get_logger().info(
                    f'Fuoco trovato! area={self._tgt_area:.0f}px → TRACKING')
                self._internal_state = self.TRACKING
                self._homing = False
                self._lock_start_time = None
                self._last_error_pan = 0.0
        elif self._internal_state == self.TRACKING:
            self._run_tracking(now)
        else:  # self.LOCKED
            self._run_locked(now)

    def _mode_callback(self, msg: Mode):
        """Transizione di stato globale: reset della macchina interna."""
        prev = self.current_mode
        self.current_mode = msg.mode
        if msg.mode == prev:
            return  # niente reset su messaggi /mode ripetuti

        if self.current_mode == Mode.FIRE:
            self.get_logger().info('Entrato in FIRE → reset a SWEEPING')
            self._reset_to_sweeping()
        else:
            self._publish_laser(False)
            self._homing = True  # il timer riporta il pan-tilt a home

    # ─── Timer: sweep / homing / failsafe ────────────────────────────────────

    def _timer_update(self):
        # Fuori da FIRE: completa solo l'homing, poi fermo
        if self.require_fire_mode and self.current_mode != Mode.FIRE:
            if self._homing:
                if self._step_home():
                    self._homing = False
                self._publish_pantilt(self._current_pan, self._current_tilt)
            return

        # Failsafe: vision muta mentre siamo in TRACKING/LOCKED → target scaduto
        if self._internal_state != self.SWEEPING:
            now = time.time()
            if (self._tgt_last_seen is None
                    or (now - self._tgt_last_seen) > self.vision_timeout):
                self.get_logger().warn(
                    'Nessuna detection valida (vision muta?) → SWEEPING')
                self._reset_to_sweeping()
            return

        # SWEEPING: prima torna a home, poi spazzola
        if self._homing:
            if self._step_home():
                self._homing = False
            self._publish_pantilt(self._current_pan, self._current_tilt)
            return

        self._sweeping_logic()
        self._publish_pantilt(self._current_pan, self._current_tilt)

    def _sweeping_logic(self):
        """Pan oscillante; a fine corsa inverte e incrementa il tilt."""
        next_pan_angle = self._current_pan + self.sweep_step * self._sweep_direction
        if self.pan_min <= next_pan_angle <= self.pan_max:
            self._current_pan = next_pan_angle
            return

        self._sweep_direction *= -1  # inverti pan

        next_tilt_angle = self._current_tilt + self.tilt_increment * self._tilt_direction
        if self.tilt_min <= next_tilt_angle <= self.tilt_max:
            self._current_tilt = next_tilt_angle
            return

        self._tilt_direction *= -1  # inverti tilt

    def _step_home(self) -> bool:
        """Un passo verso home (pan=0, tilt=tilt_min). True quando arrivato."""
        if abs(self._current_pan) <= self.sweep_step:
            self._current_pan = 0.0
        else:
            self._current_pan -= math.copysign(self.sweep_step, self._current_pan)

        dt = self._current_tilt - self.tilt_min
        if abs(dt) <= self.tilt_increment:
            self._current_tilt = self.tilt_min
        else:
            self._current_tilt -= math.copysign(self.tilt_increment, dt)

        return self._current_pan == 0.0 and self._current_tilt == self.tilt_min

    # ─── TRACKING ───────────────────────────────────────────────────────────

    def _run_tracking(self, now: float):
        """PD controller per centrare il TARGET FILTRATO al centro del frame."""
        if not self._target_valid(now):
            self.get_logger().warn(f'Fuoco perso > {self.unlock_time}s → SWEEPING')
            self._reset_to_sweeping()
            return

        ex = self._tgt_cx - self._img_w / 2.0   # px, >0 = fuoco a destra
        ey = self._tgt_cy - self._img_h / 2.0   # px, >0 = fuoco in basso
        error_mag = math.hypot(ex, ey)

        # Check lock: errore sotto soglia per lock_time → LOCKED, laser ON
        if error_mag < self.lock_threshold:
            if self._lock_start_time is None:
                self._lock_start_time = now
            elif now - self._lock_start_time >= self.lock_time:
                self.get_logger().info(
                    f'LOCK ACQUIRED! err={error_mag:.1f}px '
                    f'area={self._tgt_area:.0f}px → laser ON')
                self._internal_state = self.LOCKED
                self._lock_start_time = None
                self._publish_laser(True)
                return
        else:
            self._lock_start_time = None

        # PD controller:
        # Segno negativo: se ex>0 (fuoco a destra) il pan deve DIMINUIRE
        delta_ex = ex - self._last_error_pan
        self._last_error_pan = ex

        delta_pan = -(self.pd_kp_pan * ex + self.pd_kd * delta_ex)
        delta_tilt = -(self.pd_kp_tilt * ey)  # solo P, semplificato

        # Incrementi limitati (smooth)
        delta_pan = max(-5.0, min(5.0, delta_pan))
        delta_tilt = max(-5.0, min(5.0, delta_tilt))

        self._current_pan += delta_pan * 0.05
        self._current_tilt += delta_tilt * 0.05
        self._clamp_and_publish()

    # ─── LOCKED ─────────────────────────────────────────────────────────────

    def _run_locked(self, now: float):
        """Mantiene il lock sul centro della patch più grande. Laser ON."""
        if not self._target_valid(now):
            self.get_logger().warn('Lock perso → SWEEPING')
            self._reset_to_sweeping()
            return

        if not self._laser_on:  # belt & braces
            self._publish_laser(True)

        ex = self._tgt_cx - self._img_w / 2.0
        ey = self._tgt_cy - self._img_h / 2.0
        error_mag = math.hypot(ex, ey)

        if error_mag > self.lock_threshold:
            self.get_logger().info(f'Lock degradato (err={error_mag:.1f}px) → TRACKING')
            self._internal_state = self.TRACKING
            self._lock_start_time = None
            self._last_error_pan = ex
            self._publish_laser(False)
            return

        # Correzioni sottili per restare centrato
        self._current_pan -= ex * 0.02
        self._current_tilt -= ey * 0.02
        self._clamp_and_publish()

    # ─── Reset ──────────────────────────────────────────────────────────────

    def _reset_to_sweeping(self):
        """Reset completo allo stato SWEEPING (laser OFF, homing graduale)."""
        self._internal_state = self.SWEEPING
        self._lock_start_time = None
        self._last_error_pan = 0.0
        self._tgt_last_seen = None
        self._sweep_direction = 1
        self._tilt_direction = 1
        self._publish_laser(False)
        self._homing = True  # il timer porta a home e poi riparte lo sweep

    # ─── Publishing ──────────────────────────────────────────────────────────

    def _clamp_and_publish(self):
        self._current_pan = float(max(self.pan_min, min(self.pan_max, self._current_pan)))
        self._current_tilt = float(max(self.tilt_min, min(self.tilt_max, self._current_tilt)))
        self._publish_pantilt(self._current_pan, self._current_tilt)

    def _publish_pantilt(self, pan: float, tilt: float):
        msg = PanTiltCmd()
        msg.pan_deg = float(round(pan, 2))
        msg.tilt_deg = float(round(tilt, 2))
        msg.stamp = self.get_clock().now().to_msg()
        self._pantilt_pub.publish(msg)

    def _publish_laser(self, on: bool):
        msg = Bool()
        msg.data = on
        self._laser_on = on
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