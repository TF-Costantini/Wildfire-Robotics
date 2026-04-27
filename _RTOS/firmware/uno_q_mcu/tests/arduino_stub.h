/**
 * arduino_stub.h — Wildfire Robotics UGV
 *
 * Minimal Arduino API stub for host (x86) unit tests of the firmware
 * drivers. Replaces <Arduino.h> when building under tests/Makefile.
 *
 * Captures every digitalWrite / analogWrite / pinMode / Servo call so
 * tests can assert on the resulting pin state without flashing the MCU.
 *
 * The fake clock advances ONLY when test code calls advance_millis()
 * or advance_micros(). Drivers should never see real wall time.
 */

#ifndef ARDUINO_STUB_H
#define ARDUINO_STUB_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Arduino constants */
#define HIGH           1
#define LOW            0
#define INPUT          0
#define OUTPUT         1
#define INPUT_PULLUP   2
#define CHANGE         3
#define RISING         1
#define FALLING        2

/* Analog pin aliases — UNO-style: A0..A5 map to digital indices 14..19. */
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19

/* Arduino digital/analog/timing API */
void     pinMode(int pin, int mode);
void     digitalWrite(int pin, int value);
int      digitalRead(int pin);
void     analogWrite(int pin, int value);
uint32_t millis(void);
uint32_t micros(void);
void     delay(uint32_t ms);
void     delayMicroseconds(uint32_t us);

/* attachInterrupt / digitalPinToInterrupt — recorded but never fired */
typedef void (*isr_fn_t)(void);
int  digitalPinToInterrupt(int pin);
void attachInterrupt(int interrupt_num, isr_fn_t fn, int mode);

/* ------------------------------------------------------------------ */
/* Test harness — read/write the shared state.                         */
/* ------------------------------------------------------------------ */

#define STUB_MAX_PIN  64

/** Reset all captured state and the fake clock. */
void stub_reset(void);

/** Set what digitalRead(pin) will return until changed. */
void stub_set_digital(int pin, int value);

/** Last value passed to digitalWrite for pin (-1 if never written). */
int stub_get_digital(int pin);

/** Last value passed to analogWrite for pin (-1 if never written). */
int stub_get_analog(int pin);

/** Last mode passed to pinMode for pin (-1 if never set). */
int stub_get_mode(int pin);

/** Advance the fake clock. */
void stub_advance_millis(uint32_t ms);
void stub_advance_micros(uint32_t us);

/** Number of times digitalWrite/analogWrite was called for pin. */
int stub_count_digital(int pin);
int stub_count_analog(int pin);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ARDUINO_STUB_H */