/**
 * motor_bts7960.cpp — Wildfire Robotics UGV
 *
 * Drives two BTS7960 H-bridge modules, one per track.
 *
 * Per module the pins are:
 *   RPWM : PWM input for "forward" half-bridge
 *   LPWM : PWM input for "reverse" half-bridge
 *   EN   : single MCU GPIO that drives R_EN+L_EN tied together
 *
 * Speed mapping (-1.0 .. +1.0):
 *   v >  0   forward : analogWrite(RPWM, |v|*MAX), LPWM = 0
 *   v <  0   reverse : RPWM = 0,                    analogWrite(LPWM, |v|*MAX)
 *   v == 0   stop    : both PWM = 0
 *
 * EN is driven high in motor_init(); motor_disable() pulls it low so the
 * bridge coasts (useful for power saving in IDLE).
 */

#include "motor_bts7960.h"
#include "pins.h"

#include <Arduino.h>
#include "../src/led_matrix/LedMatrixHandler.h"

namespace {

struct State {
    float left;
    float right;
    bool  enabled;
};

State g_state = { 0.0f, 0.0f, false };

float clamp_unit(float v) {
    if (v >  1.0f) return  1.0f;
    if (v < -1.0f) return -1.0f;
    return v;
}

int duty_from_unit(float u) {
    /* u expected in [0, 1] */
    if (u <= 0.0f) return 0;
    if (u >= 1.0f) return MOTOR_DUTY_MAX;
    return static_cast<int>(u * (float)MOTOR_DUTY_MAX + 0.5f);
}

void apply_channel(int rpwm_pin, int lpwm_pin, float v) {
    v = clamp_unit(v);
    if (v >= 0.0f) {
        analogWrite(rpwm_pin, duty_from_unit(v));
        analogWrite(lpwm_pin, 0);
    } else {
        analogWrite(rpwm_pin, 0);
        analogWrite(lpwm_pin, duty_from_unit(-v));
    }
}

} /* namespace */

extern "C" {

void motor_init(void) {
    pinMode(PIN_MOTOR_A_RPWM, OUTPUT);
    pinMode(PIN_MOTOR_A_LPWM, OUTPUT);
    pinMode(PIN_MOTOR_A_EN,   OUTPUT);
    pinMode(PIN_MOTOR_B_RPWM, OUTPUT);
    pinMode(PIN_MOTOR_B_LPWM, OUTPUT);
    pinMode(PIN_MOTOR_B_EN,   OUTPUT);

    analogWrite(PIN_MOTOR_A_RPWM, 0);
    analogWrite(PIN_MOTOR_A_LPWM, 0);
    analogWrite(PIN_MOTOR_B_RPWM, 0);
    analogWrite(PIN_MOTOR_B_LPWM, 0);

    g_state.left    = 0.0f;
    g_state.right   = 0.0f;

    motor_enable();
}

void motor_set(float left, float right) {
    left  = clamp_unit(left);
    right = clamp_unit(right);
    g_state.left  = left;
    g_state.right = right;

    if (!g_state.enabled) return;

    if (left != 0) LEDMatrixHandler::left_motor_on();
    if (right != 0) LEDMatrixHandler::right_motor_on();

    apply_channel(PIN_MOTOR_A_RPWM, PIN_MOTOR_A_LPWM, left);
    apply_channel(PIN_MOTOR_B_RPWM, PIN_MOTOR_B_LPWM, right);
}

void motor_emergency_stop(void) {
    motor_disable();
    analogWrite(PIN_MOTOR_A_RPWM, 0);
    analogWrite(PIN_MOTOR_A_LPWM, 0);
    analogWrite(PIN_MOTOR_B_RPWM, 0);
    analogWrite(PIN_MOTOR_B_LPWM, 0);
    g_state.left  = 0.0f;
    g_state.right = 0.0f;
}

void motor_enable(void) {
    digitalWrite(PIN_MOTOR_A_EN, HIGH);
    digitalWrite(PIN_MOTOR_B_EN, HIGH);
    g_state.enabled = true;
}

void motor_disable(void) {
    motor_emergency_stop();
    digitalWrite(PIN_MOTOR_A_EN, LOW);
    digitalWrite(PIN_MOTOR_B_EN, LOW);
    g_state.enabled = false;
}

void motor_get_state(float *left, float *right) {
    if (left)  *left  = g_state.left;
    if (right) *right = g_state.right;
}

} /* extern "C" */