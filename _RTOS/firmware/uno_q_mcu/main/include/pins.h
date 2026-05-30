/**
 * pins.h — Wildfire Robotics UGV
 *
 * Pin assignment for Arduino UNO Q (MCU side: STM32U585AI).
 * Numbering follows the silkscreen of the UNO R3-compatible header
 * exposed by the UNO Q board (D0..D13, A0..A5). The underlying
 * STM32U585 pad mapping is resolved by the Arduino-Zephyr core
 * variant for UNO Q.
 *
 * Reserved:
 *   D0  = USART RX  (Serial / micro-ROS transport, do not use as GPIO)
 *   D1  = USART TX  (Serial / micro-ROS transport, do not use as GPIO)
 *   A4  = I2C SDA   (left free for future expansion)
 *   A5  = I2C SCL   (left free for future expansion)
 *
 * Hardware PWM-capable pins on the UNO Q header: D3, D5, D6, D9, D10, D11.
 * On STM32U585 every GPIO can fire EXTI, so HC-SR04 ECHO lines and
 * BUTTON are placed on convenient pins regardless of UNO's INT0/INT1
 * limitations on classic AVR.
 *
 * Motor driver: 2x BTS7960 (one per DC motor). Each module exposes
 *   RPWM (forward PWM input), LPWM (reverse PWM input),
 *   R_EN, L_EN (enables, tied together to a single MCU GPIO per side).
 * Direction & speed:
 *   forward: RPWM = duty, LPWM = 0
 *   reverse: RPWM = 0,    LPWM = duty
 *   stop:    RPWM = 0,    LPWM = 0
 *   disable: drive EN low to coast
 */

#ifndef PINS_H
#define PINS_H

// =====================================================================
// Motor driver A — BTS7960 #1, left track
// =====================================================================
#define PIN_MOTOR_A_RPWM        5   // PWM out, forward
#define PIN_MOTOR_A_LPWM        3   // PWM out, reverse
#define PIN_MOTOR_A_EN          4   // GPIO out, drives R_EN+L_EN tied

// =====================================================================
// Motor driver B — BTS7960 #2, right track
// =====================================================================
#define PIN_MOTOR_B_RPWM        6   // PWM out, forward
#define PIN_MOTOR_B_LPWM       11   // PWM out, reverse
#define PIN_MOTOR_B_EN          7   // GPIO out, drives R_EN+L_EN tied

// =====================================================================
// Servos MG996R — pan / tilt (50 Hz, 1-2 ms pulse)
// =====================================================================
#define PIN_SERVO_PAN           9   // PWM, driven via Servo library
#define PIN_SERVO_TILT         10   // PWM, driven via Servo library

// =====================================================================
// HC-SR04 ultrasonics — front-left and front-right
// =====================================================================
// #1 front-left
#define PIN_HCSR04_1_TRIG      A2   // GPIO out (10us trigger pulse)
#define PIN_HCSR04_1_ECHO      A3   // GPIO in,  EXTI rising+falling edge
// #2 front-right
#define PIN_HCSR04_2_TRIG      A0   // GPIO out
#define PIN_HCSR04_2_ECHO      A1   // GPIO in,  EXTI rising+falling edge

// =====================================================================
// KY-008 laser module (5 V logic, active-high)
// =====================================================================
#define PIN_LASER              13   // GPIO out (also onboard LED)

// =====================================================================
// Push button — momentary, wired to GND, internal pull-up enabled
// =====================================================================
#define PIN_BUTTON              2   // GPIO in, EXTI falling edge

// =====================================================================
// Servo timing constants (MG996R)
// =====================================================================
#define SERVO_PWM_FREQ_HZ       50      // 50 Hz frame
#define SERVO_PULSE_MIN_US      500    // 1 ms  ->   0 deg
#define SERVO_PULSE_MAX_US      2500    // 2 ms  -> 180 deg
#define SERVO_PULSE_RANGE_US    (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)

// =====================================================================
// HC-SR04 timing constants
// =====================================================================
#define HCSR04_TRIGGER_PULSE_US     10      // 10 us trigger pulse
#define HCSR04_TIMEOUT_US           25000   // ~4.3 m max range
#define HCSR04_SPEED_OF_SOUND_CM_US 0.0343  // cm/us at 20 degC

// =====================================================================
// Motor PWM constants (BTS7960)
// =====================================================================
#define MOTOR_PWM_FREQ_HZ       20000   // 20 kHz, above audible range
#define MOTOR_DUTY_MIN          0       // stop
#define MOTOR_DUTY_MAX          255     // 8-bit analogWrite full scale

// =====================================================================
// Button debounce
// =====================================================================
#define BUTTON_DEBOUNCE_MS      30      // SW debounce window after EXTI

#endif // PINS_H