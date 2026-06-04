/**
 * servo_mg996r.cpp — Wildfire Robotics UGV
 *
 * Drives the pan/tilt MG996R servos through the Arduino Servo library.
 * The library generates a 50 Hz PWM signal (20 ms frame) with a pulse
 * width between SERVO_PULSE_MIN_US and SERVO_PULSE_MAX_US, mapping the
 * input degrees onto the configured pulse range.
 *
 * Internal angle convention: 0 deg = forward / horizontal, positive
 * pan to the robot's right, positive tilt upward. Soft limits in
 * g_state are applied before each command.
 */

#include "servo_mg996r.h"
#include "pins.h"

#include <Servo.h>

#include "../led_matrix/LedMatrixHandler.h"

//Does not react if the diff between curr angle and set angle is below 2deg. For smoothing
#define DEADBAND 2.0f

namespace {

struct State {
    int pan_deg;
    int tilt_deg;
    int pan_min;
    int pan_max;
    int tilt_min;
    int tilt_max;
    int pan_offset;
    int tilt_offset;
};

State g_state = {
    /* pan_deg     */ 0,
    /* tilt_deg    */ 0,
    /* pan_min     */ -80,
    /* pan_max     */  80,
    /* tilt_min    */   -30,
    /* tilt_max    */  30,
    /* pan_offset  */  90,
    /* tilt_offset */   -15, //points slightly upwards
};

Servo g_pan;
Servo g_tilt;

int clamp(const int v, const int lo, const int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

} /* namespace */

extern "C" {

void servo_init(void) {
    g_pan.attach(PIN_SERVO_PAN);
    g_tilt.attach(PIN_SERVO_TILT);

    g_state.pan_deg  = 0;
    g_state.tilt_deg = 0;

    g_pan.write( g_state.pan_offset );
    g_tilt.write( g_state.tilt_offset );
}

void servo_pan_set(const int in) {
    const int deg = in; //clamp(in, g_state.pan_min, g_state.pan_max);

    g_state.pan_deg = deg;
    g_pan.write( (deg + g_state.pan_offset) );
}

void servo_tilt_set(const int in) {
    const int deg = clamp(in, g_state.tilt_min, g_state.tilt_max);

    g_state.tilt_deg = deg;
    g_tilt.write( (deg + g_state.tilt_offset) );
}

void servo_set_position(const int pan_deg, const int tilt_deg) {
    if (pan_deg != 0 || tilt_deg != 0)
        LEDMatrixHandler::pantilt_on();
    servo_pan_set(pan_deg);
    servo_tilt_set(tilt_deg);
}

void servo_home(void) {
    servo_set_position(0.0f, 0.0f);
}

void servo_safe_position(void) {
    /* Camera up, pan centered. Useful as a stowed/transport pose. */
    servo_set_position(0, g_state.tilt_max);
}

void servo_get_position(int *pan, int *tilt) {
    if (pan)  *pan  = g_state.pan_deg;
    if (tilt) *tilt = g_state.tilt_deg;
}

void servo_set_limits(const int pan_min, const int pan_max, const int tilt_min, const int tilt_max) {
    g_state.pan_min  = pan_min;
    g_state.pan_max  = pan_max;
    g_state.tilt_min = tilt_min;
    g_state.tilt_max = tilt_max;
    /* Re-clamp current command to the new envelope. */
    servo_set_position(g_state.pan_deg, g_state.tilt_deg);
}

} /* extern "C" */