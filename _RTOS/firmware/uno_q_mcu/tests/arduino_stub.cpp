/**
 * arduino_stub.cpp — backing storage for the host Arduino API stub.
 */

#include "arduino_stub.h"

namespace {

int      g_digital_value[STUB_MAX_PIN];   /* what digitalRead returns      */
int      g_digital_last [STUB_MAX_PIN];   /* last digitalWrite value       */
int      g_analog_last  [STUB_MAX_PIN];   /* last analogWrite value        */
int      g_pin_mode     [STUB_MAX_PIN];   /* last pinMode mode             */
int      g_digital_calls[STUB_MAX_PIN];   /* digitalWrite call counts      */
int      g_analog_calls [STUB_MAX_PIN];   /* analogWrite call counts       */
uint32_t g_millis;
uint32_t g_micros;

inline bool valid_pin(int p) {
    return p >= 0 && p < STUB_MAX_PIN;
}

} /* anonymous */

extern "C" {

void stub_reset(void) {
    for (int i = 0; i < STUB_MAX_PIN; ++i) {
        g_digital_value[i] = HIGH; /* default idle high (pull-up friendly) */
        g_digital_last [i] = -1;
        g_analog_last  [i] = -1;
        g_pin_mode     [i] = -1;
        g_digital_calls[i] = 0;
        g_analog_calls [i] = 0;
    }
    g_millis = 0;
    g_micros = 0;
}

void stub_set_digital(int pin, int value) {
    if (!valid_pin(pin)) return;
    g_digital_value[pin] = value ? HIGH : LOW;
}

int stub_get_digital(int pin) { return valid_pin(pin) ? g_digital_last[pin] : -1; }
int stub_get_analog (int pin) { return valid_pin(pin) ? g_analog_last [pin] : -1; }
int stub_get_mode   (int pin) { return valid_pin(pin) ? g_pin_mode    [pin] : -1; }

int stub_count_digital(int pin) { return valid_pin(pin) ? g_digital_calls[pin] : 0; }
int stub_count_analog (int pin) { return valid_pin(pin) ? g_analog_calls [pin] : 0; }

void stub_advance_millis(uint32_t ms) { g_millis += ms; g_micros += ms * 1000u; }
void stub_advance_micros(uint32_t us) { g_micros += us; g_millis += us / 1000u; }

/* ------------------------------------------------------------------ */
/* Arduino API surface                                                 */
/* ------------------------------------------------------------------ */

void pinMode(int pin, int mode) {
    if (!valid_pin(pin)) return;
    g_pin_mode[pin] = mode;
}

void digitalWrite(int pin, int value) {
    if (!valid_pin(pin)) return;
    g_digital_last [pin] = value ? HIGH : LOW;
    g_digital_calls[pin] += 1;
    /* Keep the readback consistent with the last write so output pins
     * read back what we drove (some drivers digitalRead their own pin). */
    g_digital_value[pin] = g_digital_last[pin];
}

int digitalRead(int pin) {
    if (!valid_pin(pin)) return LOW;
    return g_digital_value[pin];
}

void analogWrite(int pin, int value) {
    if (!valid_pin(pin)) return;
    if (value < 0)   value = 0;
    if (value > 255) value = 255;
    g_analog_last [pin] = value;
    g_analog_calls[pin] += 1;
}

uint32_t millis(void) { return g_millis; }
uint32_t micros(void) { return g_micros; }

void delay(uint32_t ms)             { g_millis += ms; g_micros += ms * 1000u; }
void delayMicroseconds(uint32_t us) { g_micros += us; g_millis += us / 1000u; }

int  digitalPinToInterrupt(int pin) { return pin; }
void attachInterrupt(int, isr_fn_t, int) { /* unused on host */ }

} /* extern "C" */
