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

#include "src/led_matrix/LedMatrixHandler.h"

extern "C" {
#include "src/drivers/button.h"
#include "src/drivers/laser_ky008.h"
#include "src/drivers/ultrasonic_hcsr04.h"
#include "src/drivers/motor_bts7960.h"
#include "src/drivers/servo_mg996r.h"
}

// =====================================================================
// Constants
// =====================================================================
#define ULTRASONIC_PERIOD_MS   300      // 10 Hz publish rate
#define HCSR04_TRIG_PERIOD_MS  200       // trigger faster than publish
#define WATCHDOG_TIMEOUT_MS    1500

#define MIN_SAFE_DISTANCE 30 // centimeters

#define BLINK_TOGGLE_COUNT    100

// =====================================================================
// Bookkeeping
// =====================================================================
static uint32_t g_last_drive_cmd_ms  = 0;
static uint32_t g_last_us_publish_ms = 0;
static uint32_t g_last_us_trigger_ms = 0;
static bool     g_drive_zeroed_by_wd = false;

bool health_row_state = true;
int health_row_skip_count = 0;

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
static bool rpc_set_drive(const float left, const float right) {
    motor_set(left, right);
    g_last_drive_cmd_ms  = millis();
    g_drive_zeroed_by_wd = false;
    return true;
}

/**
 * set_pantilt(pan_deg, tilt_deg)
 * Called by the MPU to command the pan/tilt servo.
 */
static bool rpc_set_pantilt(const float pan_deg, const float tilt_deg) {
    servo_set_position(pan_deg, tilt_deg);
    return true;
}

/**
 * set_laser(on)
 * Called by the MPU to turn the laser on or off.
 */
static bool rpc_set_laser(const bool on) {
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
static void task_publish_ultrasonics() {
    const uint32_t now = millis();

    if (now - g_last_us_trigger_ms >= HCSR04_TRIG_PERIOD_MS) {
        g_last_us_trigger_ms = now;
        hcsr04_trigger_all();
    }

    if (now - g_last_us_publish_ms >= ULTRASONIC_PERIOD_MS) {
        g_last_us_publish_ms = now;

        const float dl = hcsr04_get_left_distance();
        const float dr = hcsr04_get_right_distance();

        float left_m  = dl >= 0.0f ? dl * 0.01f : -1.0f;
        float right_m = dr >= 0.0f ? dr * 0.01f : -1.0f;

        // Fire-and-forget: we don't wait for the MPU to acknowledge.
        // If the bridge is down the call silently fails.

        //Monitor.println("PUBLISHING: Ultrasonic Data");
        LEDMatrixHandler::right_ultrasonic_on();
        LEDMatrixHandler::left_ultrasonic_on();

        Bridge.call("on_ultrasonic", left_m, right_m);
    }
}

/**
 * Poll the button driver and push a PRESS event to the MPU when
 * one is detected.
 */
static void task_publish_button() {

    if (button_process() == BUTTON_EVENT_PRESS) {
        //Monitor.println("PUBLISHING: Button Press");
        LEDMatrixHandler::button_on();

        //TODO temporary. Motor are re-enabled by pressing the same button that cycles the states
        motor_enable();

        Bridge.call("on_button", 0);
    }

}

/**
 * If no set_drive command has arrived within WATCHDOG_TIMEOUT_MS,
 * force the motors to stop. This prevents runaway if the MPU crashes
 * or the bridge drops.
 */
static void task_drive_motor_watchdog() {
    const uint32_t now = millis();
    if (now - g_last_drive_cmd_ms > WATCHDOG_TIMEOUT_MS) {
        if (!g_drive_zeroed_by_wd) {
            motor_emergency_stop();
            g_drive_zeroed_by_wd = true;
        }
    }
}

void healthIndicatorRow()
{
    //Checks and advances, if not target, returns
    if (health_row_skip_count++ != BLINK_TOGGLE_COUNT) return;

    //Resets count
    health_row_skip_count = 0;

    //Else toggles row
    health_row_state = !health_row_state;
    LEDMatrixHandler::setColInBitmap(0, health_row_state);
}

// =====================================================================
// Arduino entry points
// =====================================================================
void setup() {
    Bridge.begin();
    Monitor.begin();

    Monitor.println("MCU Setup Started");

    //Inits led matrix
    matrix.begin();

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

void checkSafetyMargin()
{
    const float dl = hcsr04_get_left_distance();
    const float dr = hcsr04_get_right_distance();

    // STOPS THE MOTOR IMMEDIATELY IF EITHER BELOW SAFETY LIMIT.
    // A negative reading (-1) means out-of-range / no echo: target far or
    // sensor blind, NOT too close. Only a VALID reading (>= 0) below the
    // limit counts as too-close — otherwise we'd brake forever when the
    // person is beyond ultrasonic range (matches follow_controller bang-bang).
    const bool left_too_close  = (dl >= 0.0f && dl < MIN_SAFE_DISTANCE);
    const bool right_too_close = (dr >= 0.0f && dr < MIN_SAFE_DISTANCE);

    if (left_too_close || right_too_close)
    {
        // Too close: block FORWARD push but ALLOW reverse so the robot can back
        // out of the safety zone. A bare emergency_stop() every loop re-zeroed
        // the reverse command from follow_controller → the robot froze here and
        // could never escape. left/right in g_state are LOGICAL (pre wiring-sign),
        // positive = forward, so clamp each wheel to <= 0.
        float l, r;
        motor_get_state(&l, &r);
        l = l < 0.0f ? l : 0.0f;
        r = r < 0.0f ? r : 0.0f;
        motor_set(l, r);
    } else
    {
        motor_enable();
    }

}

void loop() {
    // Dispatch any pending incoming RPC calls (set_drive, set_pantilt,
    // set_laser) that arrived since the last loop iteration.
    Bridge.update();

    // Always-on sensor housekeeping — independent of bridge state.
    hcsr04_update();
    task_drive_motor_watchdog();

    checkSafetyMargin();

    // Push sensor data and events to the MPU.
    task_publish_ultrasonics();
    task_publish_button();

    healthIndicatorRow();

    LEDMatrixHandler::handle_loop_decrease();
    LEDMatrixHandler::handle_loop_apply();
    LEDMatrixHandler::applyToLed(&matrix);
}