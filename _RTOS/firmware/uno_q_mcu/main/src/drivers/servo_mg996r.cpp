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

#include <Arduino.h>
#include <Arduino_RouterBridge.h>
#include <Servo.h>

#include "../led_matrix/LedMatrixHandler.h"

//Does not react if the diff between curr angle and set angle is below 2deg. For smoothing
#define DEADBAND 2.0f

namespace {

struct State {
    float pan_deg;
    float tilt_deg;
    float pan_min;
    float pan_max;
    float tilt_min;
    float tilt_max;
    float pan_offset;
    float tilt_offset;
};

State g_state = {
    /* pan_deg     */ 0.0f,
    /* tilt_deg    */ 0.0f,
    /* pan_min     */ -80.0f,
    /* pan_max     */  80.0f,
    /* tilt_min    */   0.0f,
    /* tilt_max    */  60.0f,
    /* pan_offset  */   0.0f,
    /* tilt_offset */   0.0f,
};

Servo g_pan;
Servo g_tilt;

inline float clamp(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* Map a logical angle (deg) onto the MG996R pulse range. The library
 * accepts microseconds directly via writeMicroseconds. We anchor 0 deg
 * to the midpoint of the pulse window so positive and negative angles
 * are symmetric around the servo's mechanical center. */
inline int degrees_to_us(float deg) {
    const float center = 0.5f * (SERVO_PULSE_MIN_US + SERVO_PULSE_MAX_US);
    const float us_per_deg = (float)SERVO_PULSE_RANGE_US / 180.0f;
    float us = center + deg * us_per_deg;
    if (us < SERVO_PULSE_MIN_US) us = SERVO_PULSE_MIN_US;
    if (us > SERVO_PULSE_MAX_US) us = SERVO_PULSE_MAX_US;
    return (int)us;
}

} /* namespace */

extern "C" {

void servo_init(void) {
    g_pan.attach(PIN_SERVO_PAN,  SERVO_PULSE_MIN_US, SERVO_PULSE_MAX_US);
    g_tilt.attach(PIN_SERVO_TILT, SERVO_PULSE_MIN_US, SERVO_PULSE_MAX_US);

    g_state.pan_deg  = 0.0f;
    g_state.tilt_deg = 0.0f;

    g_pan.writeMicroseconds( degrees_to_us(g_state.pan_offset) );
    g_tilt.writeMicroseconds( degrees_to_us(g_state.tilt_offset) );
}

void servo_pan_set(float deg) {
    deg = clamp(deg, g_state.pan_min, g_state.pan_max);
    if (fabsf(deg - g_state.pan_deg) < DEADBAND) return;

    g_state.pan_deg = deg;
    g_pan.writeMicroseconds( degrees_to_us(deg + g_state.pan_offset) );
}

void servo_tilt_set(float deg) {
    deg = clamp(deg, g_state.tilt_min, g_state.tilt_max);
    if (fabsf(deg - g_state.tilt_deg) < DEADBAND) return;

    g_state.tilt_deg = deg;
    g_tilt.writeMicroseconds( degrees_to_us(deg + g_state.tilt_offset) );
}

void servo_set_position(const float pan_deg, const float tilt_deg) {
    if (pan_deg != 0.0f || tilt_deg != 0.0f)
        LEDMatrixHandler::pantilt_on();
    servo_pan_set(pan_deg);
    servo_tilt_set(tilt_deg);
}

void servo_home(void) {
    servo_set_position(0.0f, 0.0f);
}

void servo_safe_position(void) {
    /* Camera up, pan centered. Useful as a stowed/transport pose. */
    servo_set_position(0.0f, g_state.tilt_max);
}

void servo_get_position(float *pan, float *tilt) {
    if (pan)  *pan  = g_state.pan_deg;
    if (tilt) *tilt = g_state.tilt_deg;
}

void servo_set_limits(float pan_min, float pan_max,
                      float tilt_min, float tilt_max) {
    g_state.pan_min  = pan_min;
    g_state.pan_max  = pan_max;
    g_state.tilt_min = tilt_min;
    g_state.tilt_max = tilt_max;
    /* Re-clamp current command to the new envelope. */
    servo_set_position(g_state.pan_deg, g_state.tilt_deg);
}

} /* extern "C" */