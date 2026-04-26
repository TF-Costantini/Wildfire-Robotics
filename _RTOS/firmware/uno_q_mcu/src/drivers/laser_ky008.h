/**
 * laser_ky008.h — Wildfire Robotics UGV
 * Public API for the KY-008 laser pointer.
 */

#ifndef LASER_KY008_H
#define LASER_KY008_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/** Configure laser GPIO as output, output low (laser off). */
void laser_init(void);

/** Drive the laser GPIO high. */
void laser_on(void);

/** Drive the laser GPIO low. */
void laser_off(void);

/** Flip the laser state. */
void laser_toggle(void);

/** True if the last commanded state is on. */
bool laser_is_on(void);

#ifdef __cplusplus
}
#endif

#endif /* LASER_KY008_H */