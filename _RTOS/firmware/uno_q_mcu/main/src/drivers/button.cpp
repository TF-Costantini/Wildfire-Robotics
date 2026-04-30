/**
 * button.cpp — Wildfire Robotics UGV
 *
 * Push-button with a robust software debounce. The button is wired
 * between PIN_BUTTON and GND, with the MCU's internal pull-up enabled,
 * so the raw line idles HIGH and reads LOW while pressed.
 *
 * Algorithm (called from button_process(), invoked frequently from
 * the main loop):
 *   1. Sample the raw level.
 *   2. If the raw level differs from the last sample, restart a
 *      debounce timer.
 *   3. Once the raw level has been stable for BUTTON_DEBOUNCE_MS,
 *      compare against the last accepted "stable" level. If it
 *      changed, update the stable level and emit one PRESS or
 *      RELEASE event.
 *
 * One event is emitted per stable transition, eliminating both
 * mechanical bounce and brief noise glitches without using a timer
 * peripheral. No EXIT is required — the state machine is purely
 * driven by polling, which keeps the ISR side empty and predictable
 */

#include "button.h"
#include "pins.h"

#include <Arduino.h>
#include <Arduino_RouterBridge.h>

#include "../led_matrix/LedMatrixHandler.h"

namespace {

struct State {
    bool     raw;             /* last sampled raw level (true = pressed) */
    bool     stable;          /* last accepted debounced level           */
    bool     previousStable;
    uint32_t last_change_ms;  /* millis() when raw last changed          */
    uint32_t last_press_ms;   /* millis() of last accepted PRESS         */
};

State g_state = { false, false, 0, 0 };

bool read_raw() {
    /* Pull-up: idle HIGH, pressed LOW. Invert so true == pressed. */
    return digitalRead(PIN_BUTTON) == LOW;
}

} /* namespace */

extern "C" {

void button_init(void) {
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    g_state.raw            = read_raw();
    g_state.stable         = g_state.raw;
    g_state.last_change_ms = millis();
    g_state.last_press_ms  = 0;
}

uint8_t button_process(void) {
    const uint32_t now = millis();
    const bool raw = read_raw();

    if (raw != g_state.raw) {
        g_state.raw            = raw;
        g_state.last_change_ms = now;
        return BUTTON_EVENT_NONE;
    }

    if ((uint32_t)(now - g_state.last_change_ms) < BUTTON_DEBOUNCE_MS) {
        return BUTTON_EVENT_NONE;
    }

    if (raw == g_state.stable) {
        return BUTTON_EVENT_NONE;
    }

    g_state.stable = raw;
    if (raw) {
        g_state.last_press_ms = now;

        // Button Change State
        if (g_state.stable != g_state.previousStable)
        {
            Monitor.println("Button: Pressed");
            LEDMatrixHandler::button_on();
        }

        g_state.previousStable = g_state.stable;
        return BUTTON_EVENT_PRESS;
    }

    g_state.previousStable = g_state.stable;
    return BUTTON_EVENT_RELEASE;
}

bool button_is_pressed(void) {
    return read_raw();
}

uint32_t button_last_press_time(void) {
    return g_state.last_press_ms;
}

} /* extern "C" */