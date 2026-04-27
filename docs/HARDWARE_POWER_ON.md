# Hardware Power-On Checklist — Wildfire Robotics UGV

Pre-flight checks to run **before** the first power-on with hardware
attached. Skipping any of these has burned us in past projects with
similar BLDC/servo loads, so treat the list as mandatory.

The pin map referenced below comes from
`_RTOS/firmware/uno_q_mcu/include/pins.h`. Re-run the static checker
after any wiring change:

```
python3 _RTOS/firmware/uno_q_mcu/tools/check_pins.py
```

---

## 1. Common ground (GND) — non-negotiable

Every power island in the rover must share a single ground reference:

- UNO Q board GND (any of the GND pins on the UNO header)
- BTS7960 #1 GND **and** BTS7960 #2 GND
- Motor battery (12 V LiPo / SLA) negative terminal
- Servo battery (or 6 V BEC) negative terminal
- HC-SR04 GND (×2)
- KY-008 laser GND
- Push-button GND

Without a common ground, the PWM duty seen by the BTS7960 floats
relative to the MCU and the motors twitch unpredictably even at duty 0.
The same is true for the servo PWM frame and for the HC-SR04 ECHO line.

**Wire it as a star, not a daisy chain.** A single thick wire from each
ground source meeting at one common node minimises voltage drop on the
return path under motor stall current.

## 2. Power rails — never share

The rover has three independent power rails. They share GND but not
positive:

| Rail | Source | Loads | Notes |
|---|---|---|---|
| 12 V motor | LiPo / SLA | BTS7960 VCC (B+) only | Direct to driver, never via the UNO Q |
| 6 V servo | dedicated BEC or 4× AA | MG996R VCC ×2 | MG996R stall ≈ 2.5 A each — must be its own rail |
| 5 V / 3.3 V logic | UNO Q USB-C or barrel | MCU, HC-SR04 VCC, KY-008, BTS7960 logic side, button pull-up | The board's regulators only |

Specifically:

- **Do NOT power MG996R from the UNO Q 5 V pin.** The board's regulator
  trips or the USB host port goes into over-current protection. The
  symptom is the board resetting every time a servo moves quickly.
- **Do NOT power the BTS7960 motor side from the UNO Q.** The driver's
  logic side (R_EN, L_EN, RPWM, LPWM) runs from the MCU's 5 V/3.3 V,
  but the B+ stud goes straight to the motor battery.
- **Common ground is enforced separately** (see §1).

## 3. Power-on order

Always:

1. **GND first** — verify continuity with a multimeter from each
   sub-system to the motor-battery negative.
2. **External rails on** — motor battery, then servo battery.
3. **USB-C / logic power last** — connect the UNO Q to the MPU.

Powering the logic before the motor rail can latch the BTS7960 into a
shoot-through state if the gate drivers see a floating output stage.

When powering off, reverse the order: USB first, then servo, then
motor.

## 4. Wiring checks against `pins.h`

Before connecting:

| Pin | Macro | Direction | Load |
|---|---|---|---|
| D2 | `PIN_BUTTON` | input pull-up (EXTI) | momentary switch to GND |
| D3 | `PIN_MOTOR_A_RPWM` | PWM out | BTS7960 #1 RPWM |
| D4 | `PIN_MOTOR_A_EN` | digital out | BTS7960 #1 R_EN+L_EN tied |
| D5 | `PIN_MOTOR_A_LPWM` | PWM out | BTS7960 #1 LPWM |
| D6 | `PIN_MOTOR_B_RPWM` | PWM out | BTS7960 #2 RPWM |
| D7 | `PIN_MOTOR_B_EN` | digital out | BTS7960 #2 R_EN+L_EN tied |
| D9 | `PIN_SERVO_PAN` | PWM (Servo lib) | MG996R signal (orange/yellow) |
| D10 | `PIN_SERVO_TILT` | PWM (Servo lib) | MG996R signal |
| D11 | `PIN_MOTOR_B_LPWM` | PWM out | BTS7960 #2 LPWM |
| D13 | `PIN_LASER` | digital out | KY-008 signal |
| A0 | `PIN_HCSR04_1_TRIG` | digital out | HC-SR04 left TRIG |
| A1 | `PIN_HCSR04_1_ECHO` | digital in (EXTI both edges) | HC-SR04 left ECHO **(see §5)** |
| A2 | `PIN_HCSR04_2_TRIG` | digital out | HC-SR04 right TRIG |
| A3 | `PIN_HCSR04_2_ECHO` | digital in (EXTI both edges) | HC-SR04 right ECHO **(see §5)** |

Reserved (do not use): D0/D1 = USART TX/RX (micro-ROS transport).

## 5. HC-SR04 ECHO level shifter

The HC-SR04 ECHO output swings to **5 V**. The STM32U585 GPIO inside
the UNO Q is **3.3 V tolerant — not 5 V tolerant on most pads**.
Connecting ECHO directly will eventually damage the input.

Use a resistor divider on each ECHO line:

```
ECHO ──[ 1 kΩ ]──┬── MCU pin (A1 or A3)
                 │
                [ 2 kΩ ]
                 │
                GND
```

This brings 5 V → 3.33 V at the MCU side. TRIG can be driven directly
(MCU 3.3 V is enough to clear the HC-SR04's HIGH threshold).

## 6. BTS7960 wiring

Each module:

| Module pin | To |
|---|---|
| VCC (logic) | UNO Q 5 V |
| GND (logic) | common GND |
| R_EN, L_EN | tied together → MCU `_EN` pin (D4 or D7) |
| RPWM | MCU `_RPWM` pin (D3 or D6) |
| LPWM | MCU `_LPWM` pin (D5 or D11) |
| B+ (motor) | motor battery + |
| B- (motor) | motor battery − |
| M+ / M− | DC motor leads |

The driver supports PWM up to 25 kHz. Firmware uses 20 kHz
(`MOTOR_PWM_FREQ_HZ`) to stay above the audible band.

## 7. Servo MG996R wiring

| MG996R | To |
|---|---|
| Brown / black (GND) | common GND |
| Red (VCC) | dedicated 6 V BEC + |
| Orange / yellow (signal) | MCU D9 (pan) or D10 (tilt) |

A 100 µF electrolytic across each servo's VCC/GND pins right at the
servo helps absorb the inrush when the gear motor starts.

## 8. KY-008 laser

| KY-008 | To |
|---|---|
| Signal (S) | MCU D13 |
| VCC (middle pin) | 5 V |
| GND (−) | common GND |

Active-high — `laser_on()` writes `HIGH`. The driver guarantees `LOW`
at boot. Never look directly into the beam, and confirm the FIRE state
machine has actually `LOCKED` before allowing the laser to come on
during testing — the gating is enforced in `state_machine_node.py`,
which only forwards `/cmd_laser` to `/cmd_laser_mcu` while in
`Mode.FIRE`.

## 9. Push button

Single momentary switch, two terminals:

```
D2 ──── [ button ] ──── GND
```

Internal pull-up enabled in `button_init()`. **No external resistor
required.** Polling + 30 ms software debounce in `button_process()`.

## 10. First power-on procedure

1. **Disconnect motor leads** from the BTS7960 outputs (M+ / M−).
2. **Disconnect servo signal** wires from D9/D10 (or unplug the servo
   power so they sit limp).
3. Power on motor + servo rails per §3.
4. Plug USB-C from UNO Q to MPU.
5. Flash the firmware:
   ```
   pio run -e uno_q_mcu -t upload
   ```
6. Start the agent + stack:
   ```
   ros2 launch wildfire_bringup bringup.launch.py agent_dev:=/dev/ttyACM0
   ```
   (macOS users: see §11.)
7. Verify topics appear:
   ```
   ros2 topic list
   ros2 topic echo /ultrasonic/left
   ros2 topic echo /button_event   # press the button
   ```
8. With motors and servo signal still disconnected, command movement
   and watch the log of the firmware (`pio device monitor`) and of the
   `state_machine_node` to confirm the gating works as expected.
9. Reconnect servo signal first, command small (5–10°) pan motions to
   verify direction. Reconnect motor leads last; start with low duty
   (`/cmd_drive_mcu` left=right=0.2) on a stand with the wheels off the
   ground.
10. Validate the watchdog: kill the controller node and confirm the
    motors stop within 500 ms.

## 11. Serial port path on macOS

When the UNO Q enumerates on a Mac (the MPU is the Dragonwing on the
board itself, but macOS shows up if you plug the same USB-C into a
laptop for direct firmware flashing/debug), the serial path is
`/dev/tty.usbmodemNNNN`, not `/dev/ttyACM0`. Override the launch arg:

```
ros2 launch wildfire_bringup bringup.launch.py \
    agent_dev:=/dev/tty.usbmodem1101
```

Use `ls /dev/tty.usbmodem*` to discover the live name.

## 12. Sanity-check failure modes

| Symptom | Likely cause |
|---|---|
| Motors twitch at duty 0 | floating BTS7960 PWM input — check `motor_init()` ran (RPWM/LPWM written to 0) and ground continuity |
| Board resets when servo moves | servo drawing from UNO Q 5 V (§2). Move servo VCC to dedicated BEC |
| ECHO reads always 0 cm or NaN | divider missing or wired wrong; or HC-SR04 GND not common |
| `Watchdog timeout` repeating | controller node not publishing `/cmd_*`; check `state_machine_node` is in the expected mode |
| Agent shows `errno: 25` | `agent_dev` path is not a serial device; verify with `ls /dev/tty*` |
| Laser stays off in FIRE | controller has not yet entered `LOCKED` state; check `min_fire_area`, `lock_threshold_px`, `lock_time_s` in `params.yaml` |

## 13. Static checks to run before each session

```
# Pin map
python3 _RTOS/firmware/uno_q_mcu/tools/check_pins.py

# Firmware compiles
pio run -e uno_q_mcu

# Driver host tests
make -C _RTOS/firmware/uno_q_mcu/tests run

# Vision/control stack builds
cd _LINUX/src/ros2_ws && colcon build && source install/setup.bash
```

If any of those fails, do **not** apply power.
