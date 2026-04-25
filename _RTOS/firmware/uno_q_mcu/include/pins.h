/**
 * pins.h — Wildfire Robotics UGV
 * TODO: Definire tutti i pinout per Arduino Uno Q (MCU STM32U585)
 *
 * ASSEGNAZIONE PIN (da confermare con datasheet Arduino Uno Q):
 *
 * Motor Driver TB6612FNG:
 *   - AIN1: GPIO output
 *   - AIN2: GPIO output
 *   - PWMA: PWM output
 *   - BIN1: GPIO output
 *   - BIN2: GPIO output
 *   - PWMB: PWM output
 *   - STBY: GPIO output
 *
 * Servo Pan MG996R:
 *   - PWM 50Hz (1-2ms duty cycle per 0-180°)
 *
 * Servo Tilt MG996R:
 *   - PWM 50Hz (1-2ms duty cycle per 0-180°)
 *
 * HC-SR04 #1 (front-left):
 *   - TRIG: GPIO output
 *   - ECHO: GPIO input (con interrupt)
 *
 * HC-SR04 #2 (front-right):
 *   - TRIG: GPIO output
 *   - ECHO: GPIO input (con interrupt)
 *
 * KY-008 Laser:
 *   - SIG: GPIO output
 *
 * Push Button:
 *   - GPIO input con pull-up interno
 */

#ifndef PINS_H
#define PINS_H

// === MCU STM32U585 (Arduino Uno Q) ===
// TODO: Verificare i pin reali sul datasheet

// Motor Driver TB6612FNG
#define PIN_MOTOR_AIN1      0  // TODO: Assegnare pin corretto
#define PIN_MOTOR_AIN2      0  // TODO: Assegnare pin corretto
#define PIN_MOTOR_PWMA      0  // TODO: Assegnare pin PWM corretto
#define PIN_MOTOR_BIN1      0  // TODO: Assegnare pin corretto
#define PIN_MOTOR_BIN2      0  // TODO: Assegnare pin corretto
#define PIN_MOTOR_PWMB      0  // TODO: Assegnare pin PWM corretto
#define PIN_MOTOR_STBY      0  // TODO: Assegnare pin corretto

// Servo Pan (GPIO + PWM timer)
#define PIN_SERVO_PAN       0  // TODO: Assegnare pin PWM corretto (50Hz)
#define PIN_SERVO_TILT      0  // TODO: Assegnare pin PWM corretto (50Hz)

// HC-SR04 #1 (front-left)
#define PIN_HCSR04_1_TRIG   0  // TODO: Assegnare pin corretto
#define PIN_HCSR04_1_ECHO   0  // TODO: Assegnare pin (conEXTI interrupt)

// HC-SR04 #2 (front-right)
#define PIN_HCSR04_2_TRIG   0  // TODO: Assegnare pin corretto
#define PIN_HCSR04_2_ECHO   0  // TODO: Assegnare pin (conEXTI interrupt)

// KY-008 Laser
#define PIN_LASER           0  // TODO: Assegnare pin corretto

// Push Button (con pull-up interno)
#define PIN_BUTTON          0  // TODO: Assegnare pin corretto (con_EXTI)

// === COSTANTI PWM ===
#define SERVO_PWM_FREQ_HZ       50      // 50Hz per MG996R
#define SERVO_PULSE_MIN_US      1000    // 1ms = 0°
#define SERVO_PULSE_MAX_US      2000    // 2ms = 180°
#define SERVO_PULSE_RANGE_US    (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US)

// === COSTANTI HC-SR04 ===
#define HCSR04_TRIGGER_PULSE_US     10      // 10μs trigger pulse
#define HCSR04_TIMEOUT_US           25000   // ~4.3m max range (25ms timeout)
#define HCSR04_SPEED_OF_SOUND_CM_US 0.0343  // cm/μs (a 20°C)

// === COSTANTI MOTOR ===
#define MOTOR_PWM_FREQ_HZ       20000  // 20kHz PWM per TB6612FNG
#define MOTOR_SPEED_MIN         0      // Stop
#define MOTOR_SPEED_MAX         100    // Full speed

#endif // PINS_H
