/**
 * laser_ky008.cpp — Wildfire Robotics UGV
 *
 * Trivial GPIO driver for the KY-008 laser module. Active-high logic:
 * driving the signal pin high turns the diode on. Always driven low
 * at init time to guarantee an off state at power-up.
 *
 * Safety: the FIRE controller above MUST gate this on confirmed lock.
 * This driver enforces no policy — it just toggles the pin.
 */

#include "laser_ky008.h"
#include "pins.h"

#include <Arduino.h>

namespace {
volatile bool g_on = false;
}

extern "C" {

void laser_init(void) {
    pinMode(PIN_LASER, OUTPUT);
    digitalWrite(PIN_LASER, LOW);
    g_on = false;
}

void laser_on(void) {
    digitalWrite(PIN_LASER, HIGH);
    g_on = true;
}

void laser_off(void) {
    digitalWrite(PIN_LASER, LOW);
    g_on = false;
}

void laser_toggle(void) {
    if (g_on) laser_off();
    else      laser_on();
}

bool laser_is_on(void) {
    return g_on;
}

} /* extern "C" */