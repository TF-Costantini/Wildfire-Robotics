/**
 * servo_mg996r.h — Wildfire Robotics UGV
 * Public API for the pan/tilt MG996R servo pair.
 */

#ifndef SERVO_MG996R_H
#define SERVO_MG996R_H

#ifdef __cplusplus
extern "C" {
#endif

/** Attach pan and tilt servos, configure 50 Hz PWM, move to home (0,0). */
void servo_init(void);

/** Set pan angle in degrees (clamped to configured limits). */
void servo_pan_set(int in);

/** Set tilt angle in degrees (clamped to configured limits). */
void servo_tilt_set(int in);

/** Set pan and tilt at once. */
void servo_set_position(int pan_deg, int tilt_deg);

/** Move both servos to (0, 0). */
void servo_home(void);

/** Move both servos to a safe stowed position. */
void servo_safe_position(void);

/** Read back last commanded angles. NULL pointers are skipped. */
void servo_get_position(int *pan, int *tilt);

/** Override the default soft limits. */
void servo_set_limits(int pan_min, int pan_max,
                      int tilt_min, int tilt_max);

#ifdef __cplusplus
}
#endif

#endif /* SERVO_MG996R_H */