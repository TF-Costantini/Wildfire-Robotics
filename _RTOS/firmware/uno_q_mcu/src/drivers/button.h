/**
 * button.h — Wildfire Robotics UGV
 * Public API for the mode-cycle push button.
 */

#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define BUTTON_EVENT_NONE     0
#define BUTTON_EVENT_PRESS    1
#define BUTTON_EVENT_RELEASE  2

/** Configure GPIO as input pull-up; reset internal state. */
void button_init(void);

/**
 * Process the button. Call from the main loop frequently
 * (e.g. >= 100 Hz). Performs software debounce and emits one
 * event per stable transition.
 *
 * @return BUTTON_EVENT_NONE / _PRESS / _RELEASE
 */
uint8_t button_process(void);

/** Raw, undebounced read (true if currently pressed). */
bool button_is_pressed(void);

/** millis() timestamp of the last debounced press event. */
uint32_t button_last_press_time(void);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */