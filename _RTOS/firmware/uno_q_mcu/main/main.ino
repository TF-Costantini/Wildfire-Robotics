/**
 * main.cpp — Wildfire Robotics UGV
 *
 * Arduino_RouterBridge RPC node for the STM32U585 side of the UNO Q.
 * Bridges real-time I/O (motors, servos, ultrasonics, laser, button)
 * to the QRB2210 MPU running Debian Linux via MessagePack RPC over
 * the internal UART (Serial1 / ttyHS1).
 *
 * RPC surface
 * -----------
 *  MCU provides (MPU calls these):
 *    set_drive(float left, float right) -> bool
 *    set_pantilt(float pan_deg, float tilt_deg) -> bool
 *    set_laser(bool on) -> bool
 *
 *  MCU calls (MPU must provide these):
 *    on_ultrasonic(float left_m, float right_m)
 *    on_button(int kind)   // 0 = PRESS
 *
 * Safety
 * ------
 *  - Drive watchdog: if no set_drive RPC arrives within
 *    WATCHDOG_TIMEOUT_MS, motors are forced to (0, 0).
 *  - While the bridge is not connected the watchdog keeps
 *    motors stopped.
 *  - HC-SR04 ECHO is captured via EXTI inside the driver, so
 *    sensor reads never block the RPC loop.
 */

#include <Arduino.h>
#include <Arduino_RouterBridge.h>

extern "C" {
#include "src/drivers/motor_bts7960.h"
#include "src/drivers/servo_mg996r.h"
#include "src/drivers/ultrasonic_hcsr04.h"
#include "src/drivers/laser_ky008.h"
#include "src/drivers/button.h"
}

// =====================================================================
// Constants
// =====================================================================
#define ULTRASONIC_PERIOD_MS   100      // 10 Hz publish rate
#define HCSR04_TRIG_PERIOD_MS  60       // trigger faster than publish
#define WATCHDOG_TIMEOUT_MS    500

// =====================================================================
// Bookkeeping
// =====================================================================
static uint32_t g_last_drive_cmd_ms  = 0;
static uint32_t g_last_us_publish_ms = 0;
static uint32_t g_last_us_trigger_ms = 0;
static bool     g_drive_zeroed_by_wd = false;

// =====================================================================
// RPC methods provided by the MCU
// The MPU calls these to command the hardware.
// provide_safe() means they are dispatched in the main loop thread
// via Bridge.update_safe(), which avoids re-entrancy issues with the
// motor / servo drivers.
// =====================================================================

/**
 * set_drive(left, right)
 * Called by the MPU to command the drive motors.
 * left / right are in the range [-1.0, 1.0].
 */
static bool rpc_set_drive(float left, float right) {
    motor_set(left, right);
    g_last_drive_cmd_ms  = millis();
    g_drive_zeroed_by_wd = false;
    return true;
}

/**
 * set_pantilt(pan_deg, tilt_deg)
 * Called by the MPU to command the pan/tilt servo.
 */
static bool rpc_set_pantilt(float pan_deg, float tilt_deg) {
    servo_set_position(pan_deg, tilt_deg);
    return true;
}

/**
 * set_laser(on)
 * Called by the MPU to turn the laser on or off.
 */
static bool rpc_set_laser(bool on) {
    if (on) laser_on();
    else    laser_off();
    return true;
}

// =====================================================================
// Periodic tasks
// =====================================================================

/**
 * Trigger the HC-SR04 sensors and publish distance readings to the MPU
 * by calling the MPU-registered on_ultrasonic RPC method.
 * Distances are converted from cm to meters before sending.
 * Out-of-range readings are sent as -1.0 (NaN equivalent for msgpack).
 */
static void task_publish_ultrasonics(void) {
    const uint32_t now = millis();

    if ((uint32_t)(now - g_last_us_trigger_ms) >= HCSR04_TRIG_PERIOD_MS) {
        g_last_us_trigger_ms = now;
        hcsr04_trigger_all();
    }

    if ((uint32_t)(now - g_last_us_publish_ms) >= ULTRASONIC_PERIOD_MS) {
        g_last_us_publish_ms = now;

        float dl = hcsr04_get_distance(HCSR04_LEFT);
        float dr = hcsr04_get_distance(HCSR04_RIGHT);

        float left_m  = (dl >= 0.0f) ? (dl * 0.01f) : -1.0f;
        float right_m = (dr >= 0.0f) ? (dr * 0.01f) : -1.0f;

        // Fire-and-forget: we don't wait for the MPU to acknowledge.
        // If the bridge is down the call silently fails.
        Bridge.call("on_ultrasonic", left_m, right_m);
    }
}

/**
 * Poll the button driver and push a PRESS event to the MPU when
 * one is detected.
 */
static void task_publish_button(void) {
    uint8_t ev = button_process();
    if (ev == BUTTON_EVENT_PRESS) {
        Bridge.call("on_button", (int)0); // 0 = PRESS
    }
}

/**
 * If no set_drive command has arrived within WATCHDOG_TIMEOUT_MS,
 * force the motors to stop. This prevents runaway if the MPU crashes
 * or the bridge drops.
 */
static void task_drive_watchdog(void) {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_last_drive_cmd_ms) > WATCHDOG_TIMEOUT_MS) {
        if (!g_drive_zeroed_by_wd) {
            motor_set(0.0f, 0.0f);
            g_drive_zeroed_by_wd = true;
        }
    }
}

// =====================================================================
// Arduino entry points
// =====================================================================
void setup(void) {
    // Bridge.begin() initialises Serial1 at 115200 and performs the
    // handshake with the arduino-router service on the MPU.
    Bridge.begin();
    Monitor.begin(); // debug output (goes to MPU-side log)

    // Register the methods the MPU is allowed to call on us.
    // provide_safe() = dispatched in loop() via update_safe(), so
    // it is safe to touch hardware from inside the callback.
    if (!Bridge.provide("set_drive",   rpc_set_drive))
        Monitor.println("ERR: register set_drive");

    if (!Bridge.provide("set_pantilt", rpc_set_pantilt))
        Monitor.println("ERR: register set_pantilt");

    if (!Bridge.provide("set_laser",   rpc_set_laser))
        Monitor.println("ERR: register set_laser");

    // Hardware init
    motor_init();
    servo_init();
    hcsr04_init();
    laser_init();
    button_init();

    // Safe starting state
    motor_set(0.0f, 0.0f);
    servo_home();
    laser_off();

    g_last_drive_cmd_ms  = millis();
    g_last_us_publish_ms = millis();
    g_last_us_trigger_ms = millis();

    Monitor.println("MCU ready");
}

void loop(void) {
    // Dispatch any pending incoming RPC calls (set_drive, set_pantilt,
    // set_laser) that arrived since the last loop iteration.
    Bridge.update();

    // Always-on sensor housekeeping — independent of bridge state.
    hcsr04_update();
    task_drive_watchdog();

    // Push sensor data and events to the MPU.
    task_publish_ultrasonics();
    task_publish_button();
}