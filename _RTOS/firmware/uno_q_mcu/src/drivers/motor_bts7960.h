/**
 * motor_bts7960.h — Wildfire Robotics UGV
 * Public API for the BTS7960 motor driver pair (left + right tracks).
 */

#ifndef MOTOR_BTS7960_H
#define MOTOR_BTS7960_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/** Init both BTS7960 modules; pins as outputs, EN high, PWM at 0. */
void motor_init(void);

/**
 * Set normalized speed for both tracks.
 * @param left  in [-1.0, +1.0]; positive = forward
 * @param right in [-1.0, +1.0]; positive = forward
 * Values outside the range are clamped.
 */
void motor_set(float left, float right);

/** Bring both PWM outputs to 0 immediately. EN stays high. */
void motor_emergency_stop(void);

/** Drive R_EN+L_EN high on both modules (motors active). */
void motor_enable(void);

/** Drive R_EN+L_EN low on both modules (motors coast/disabled). */
void motor_disable(void);

/** Read back the last commanded normalized values. NULL pointers are skipped. */
void motor_get_state(float *left, float *right);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_BTS7960_H */