/**
 * ultrasonic_hcsr04.h — Wildfire Robotics UGV
 * Public API for the front-left / front-right HC-SR04 pair.
 */

#ifndef ULTRASONIC_HCSR04_H
#define ULTRASONIC_HCSR04_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define HCSR04_LEFT   0
#define HCSR04_RIGHT  1

/** Configure TRIG as output, ECHO as input, attach EXTI on both edges. */
void hcsr04_init(void);

/** Fire a 10 us trigger pulse on the given sensor (id = 0/1). */
void hcsr04_trigger(uint8_t id);

/** Trigger both sensors back-to-back (with a small spacing). */
void hcsr04_trigger_all(void);

/** Periodic housekeeping: invalidate readings older than HCSR04_TIMEOUT_US. */
void hcsr04_update(void);

/** Last valid distance in cm (id = 0/1). Returns -1.0f if not valid. */
float hcsr04_get_distance(uint8_t id);

/** True if last reading is valid and within timeout. */
bool hcsr04_is_valid(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* ULTRASONIC_HCSR04_H */