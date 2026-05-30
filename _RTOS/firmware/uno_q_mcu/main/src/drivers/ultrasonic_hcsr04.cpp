/**
 * ultrasonic_hcsr04.cpp — Wildfire Robotics UGV
 *
 * Non-blocking driver for two HC-SR04 sensors. The TRIG line is pulsed
 * for 10 us (a brief delayMicroseconds is acceptable: it never blocks
 * for more than the trigger pulse). Echo width is captured entirely in
 * an EXTI ISR using micros() timestamps, so the main loop and the
 * micro-ROS executor never wait on the sensor.
 *
 * Per-sensor lifecycle:
 *   IDLE   -> trigger() -> WAITING_RISE
 *   ECHO rising edge ISR  -> WAITING_FALL (records echo_start)
 *   ECHO falling edge ISR -> IDLE         (records echo_end, computes cm)
 *
 * Stale captures (e.g. lost falling edge, target out of range) are
 * cleared by hcsr04_update(): if a sensor has been WAITING_* longer
 * than HCSR04_TIMEOUT_US it is reset and marked invalid.
 */

#include "ultrasonic_hcsr04.h"
#include "pins.h"

#include <Arduino.h>

namespace {

enum SensorPhase : uint8_t {
    PHASE_IDLE         = 0,
    PHASE_WAIT_RISE    = 1,
    PHASE_WAIT_FALL    = 2,
};

struct Sensor {
    uint8_t  trig_pin;
    uint8_t  echo_pin;
    volatile uint8_t  phase;
    volatile uint32_t trig_us;     /* time of TRIG pulse end                */
    volatile uint32_t echo_start;  /* micros() at ECHO rising edge          */
    volatile uint32_t echo_end;    /* micros() at ECHO falling edge         */
    volatile bool     valid;
    volatile float    distance_cm;
};

Sensor g_sensor[2] = {
    { PIN_HCSR04_1_TRIG, PIN_HCSR04_1_ECHO, PHASE_IDLE, 0, 0, 0, false, 0.0f },
    { PIN_HCSR04_2_TRIG, PIN_HCSR04_2_ECHO, PHASE_IDLE, 0, 0, 0, false, 0.0f },
};

void echo_isr(uint8_t id) {
    Sensor &s = g_sensor[id];

    /* digitalRead() inside an ISR is acceptable on Arduino-STM32 (it
     * just reads the GPIO input data register). */

    const bool high = digitalRead(s.echo_pin) == HIGH;
    const uint32_t now = micros();

    if (high) {
        if (s.phase == PHASE_WAIT_RISE) {
            s.echo_start = now;
            s.phase      = PHASE_WAIT_FALL;
        }
    } else {
        if (s.phase == PHASE_WAIT_FALL) {
            s.echo_end = now;
            s.phase    = PHASE_IDLE;
            const auto width = s.echo_end - s.echo_start;
            if (width > 0 && width < HCSR04_TIMEOUT_US) {
                s.distance_cm = (static_cast<float>(width) * 0.5f) *
                                static_cast<float>(HCSR04_SPEED_OF_SOUND_CM_US);
                s.valid = true;
            } else {
                s.valid = false;
            }
        }
    }
}

void echo_isr_left()  { echo_isr(HCSR04_LEFT);  }
void echo_isr_right() { echo_isr(HCSR04_RIGHT); }

} /* namespace */

extern "C" {

void hcsr04_init(void) {
    for (int i = 0; i < 2; ++i) {
        pinMode(g_sensor[i].trig_pin, OUTPUT);
        pinMode(g_sensor[i].echo_pin, INPUT);
        digitalWrite(g_sensor[i].trig_pin, LOW);
        g_sensor[i].phase       = PHASE_IDLE;
        g_sensor[i].valid       = false;
        g_sensor[i].distance_cm = 0.0f;
    }
    attachInterrupt(digitalPinToInterrupt(PIN_HCSR04_1_ECHO),
                    echo_isr_left,  CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_HCSR04_2_ECHO),
                    echo_isr_right, CHANGE);
}

void hcsr04_trigger(uint8_t id) {
    if (id > 1) return;
    Sensor &s = g_sensor[id];

    digitalWrite(s.trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(s.trig_pin, HIGH);
    delayMicroseconds(HCSR04_TRIGGER_PULSE_US);
    digitalWrite(s.trig_pin, LOW);

    s.trig_us = micros();
    s.phase   = PHASE_WAIT_RISE;
}

void hcsr04_trigger_all(void) {
    hcsr04_trigger(HCSR04_LEFT);
    /* Small gap to limit cross-talk between the two transducers. */
    delayMicroseconds(200);
    hcsr04_trigger(HCSR04_RIGHT);
}

void hcsr04_update(void) {
    const uint32_t now = micros();
    for (auto & s : g_sensor) {
        if (s.phase != PHASE_IDLE) {
            const auto elapsed = now - s.trig_us;
            if (elapsed > HCSR04_TIMEOUT_US) {
                s.phase = PHASE_IDLE;
                s.valid = false;
            }
        }
    }
}

float hcsr04_get_distance(const uint8_t id) {
    if (id > 1)              return -1.0f;
    if (!g_sensor[id].valid) return -1.0f;
    return g_sensor[id].distance_cm;
}

float hcsr04_get_left_distance(void)
{
    return hcsr04_get_distance(HCSR04_RIGHT);
}

float hcsr04_get_right_distance(void)
{
    return hcsr04_get_distance(HCSR04_RIGHT);
}


bool hcsr04_is_valid(const uint8_t id) {
    if (id > 1) return false;
    return g_sensor[id].valid;
}

} /* extern "C" */