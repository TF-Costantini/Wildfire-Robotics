/**
 * test_drivers.cpp — host unit tests for Wildfire firmware drivers.
 *
 * Compiled by tests/Makefile against arduino_stub.{h,cpp} and a Servo
 * shim. Tests assert on the captured pin states after each driver
 * call. ISR-driven paths (HC-SR04) are not exercised here because the
 * stub's attachInterrupt does not replay edges; the trigger pulse
 * shape is checked instead.
 */

#include "arduino_stub.h"
#include "pins.h"

extern "C" {
#include "drivers/motor_bts7960.h"
#include "drivers/servo_mg996r.h"
#include "drivers/button.h"
#include "drivers/laser_ky008.h"
#include "drivers/ultrasonic_hcsr04.h"
}

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

static int g_pass = 0;
static int g_fail = 0;

#define CHECK(cond, msg) do {                                \
    if (cond) { ++g_pass; }                                  \
    else {                                                   \
        ++g_fail;                                            \
        std::fprintf(stderr,                                 \
            "FAIL %s:%d  %s\n", __FILE__, __LINE__, msg);    \
    }                                                        \
} while (0)

#define CHECK_EQ(a, b, msg) do {                                       \
    auto _a = (a); auto _b = (b);                                      \
    if (_a == _b) { ++g_pass; }                                        \
    else {                                                             \
        ++g_fail;                                                      \
        std::fprintf(stderr, "FAIL %s:%d  %s  (got %lld, want %lld)\n",\
            __FILE__, __LINE__, msg,                                   \
            (long long)_a, (long long)_b);                             \
    }                                                                  \
} while (0)

#define CHECK_NEAR(a, b, eps, msg) do {                                \
    double _a = (double)(a); double _b = (double)(b);                  \
    if (std::fabs(_a - _b) <= (eps)) { ++g_pass; }                     \
    else {                                                             \
        ++g_fail;                                                      \
        std::fprintf(stderr, "FAIL %s:%d  %s  (got %g, want %g)\n",    \
            __FILE__, __LINE__, msg, _a, _b);                          \
    }                                                                  \
} while (0)


/* =================================================================== */
/* MOTOR                                                                */
/* =================================================================== */

static void test_motor_init_drives_zero_and_enables(void) {
    stub_reset();
    motor_init();

    CHECK_EQ(stub_get_mode(PIN_MOTOR_A_RPWM), OUTPUT, "A_RPWM mode");
    CHECK_EQ(stub_get_mode(PIN_MOTOR_A_LPWM), OUTPUT, "A_LPWM mode");
    CHECK_EQ(stub_get_mode(PIN_MOTOR_A_EN),   OUTPUT, "A_EN mode");
    CHECK_EQ(stub_get_mode(PIN_MOTOR_B_RPWM), OUTPUT, "B_RPWM mode");
    CHECK_EQ(stub_get_mode(PIN_MOTOR_B_LPWM), OUTPUT, "B_LPWM mode");
    CHECK_EQ(stub_get_mode(PIN_MOTOR_B_EN),   OUTPUT, "B_EN mode");

    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_RPWM), 0, "A_RPWM 0");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_LPWM), 0, "A_LPWM 0");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_RPWM), 0, "B_RPWM 0");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_LPWM), 0, "B_LPWM 0");

    CHECK_EQ(stub_get_digital(PIN_MOTOR_A_EN), HIGH, "A_EN high");
    CHECK_EQ(stub_get_digital(PIN_MOTOR_B_EN), HIGH, "B_EN high");
}

static void test_motor_set_forward(void) {
    stub_reset();
    motor_init();

    motor_set(0.5f, 1.0f);

    /* duty = round(0.5 * 255) = 128 */
    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_RPWM), 128, "left forward duty");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_LPWM),   0, "left reverse off");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_RPWM), 255, "right forward full");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_LPWM),   0, "right reverse off");
}

static void test_motor_set_reverse(void) {
    stub_reset();
    motor_init();

    motor_set(-0.25f, -1.0f);

    /* duty = round(0.25 * 255) = 64 */
    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_RPWM),   0, "left forward off");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_LPWM),  64, "left reverse duty");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_RPWM),   0, "right forward off");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_LPWM), 255, "right reverse full");
}

static void test_motor_set_clamps_oversaturated_inputs(void) {
    stub_reset();
    motor_init();

    motor_set(2.0f, -3.0f);

    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_RPWM), 255, "L>1 clamps to forward full");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_LPWM),   0, "L>1 reverse stays 0");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_RPWM),   0, "R<-1 forward stays 0");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_LPWM), 255, "R<-1 clamps to reverse full");
}

static void test_motor_emergency_stop(void) {
    stub_reset();
    motor_init();
    motor_set(1.0f, -1.0f);

    motor_emergency_stop();

    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_RPWM), 0, "A_RPWM stopped");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_A_LPWM), 0, "A_LPWM stopped");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_RPWM), 0, "B_RPWM stopped");
    CHECK_EQ(stub_get_analog(PIN_MOTOR_B_LPWM), 0, "B_LPWM stopped");

    float l = -9, r = -9;
    motor_get_state(&l, &r);
    CHECK_NEAR(l, 0.0f, 1e-6, "state.left zeroed");
    CHECK_NEAR(r, 0.0f, 1e-6, "state.right zeroed");
}

static void test_motor_disable_drops_en_and_ignores_set(void) {
    stub_reset();
    motor_init();

    motor_disable();
    CHECK_EQ(stub_get_digital(PIN_MOTOR_A_EN), LOW, "A_EN low after disable");
    CHECK_EQ(stub_get_digital(PIN_MOTOR_B_EN), LOW, "B_EN low after disable");

    /* When disabled, motor_set() must not drive the bridges. */
    int prev_a = stub_count_analog(PIN_MOTOR_A_RPWM);
    motor_set(1.0f, 1.0f);
    int now_a  = stub_count_analog(PIN_MOTOR_A_RPWM);
    CHECK_EQ(now_a - prev_a, 0, "set ignored while disabled");
}

/* =================================================================== */
/* SERVO                                                                */
/* =================================================================== */

/* The Servo stub is in tests/Servo.h. servo_mg996r.cpp instantiates
 * static Servo objects in its own translation unit. We can't reach
 * them directly, so we assert on the public API (clamping, get_position)
 * and on the pulse range bounds (1500 us at 0 deg by construction). */

static void test_servo_pan_clamps_to_limits(void) {
    stub_reset();
    servo_init();

    servo_pan_set(200.0f);
    float pan = -1.0f, tilt = -1.0f;
    servo_get_position(&pan, &tilt);
    CHECK_NEAR(pan, 60.0f, 1e-3, "pan clamped to +60");

    servo_pan_set(-200.0f);
    servo_get_position(&pan, &tilt);
    CHECK_NEAR(pan, -60.0f, 1e-3, "pan clamped to -60");
}

static void test_servo_tilt_clamps_to_limits(void) {
    stub_reset();
    servo_init();

    servo_tilt_set(100.0f);
    float pan = 0.0f, tilt = 0.0f;
    servo_get_position(&pan, &tilt);
    CHECK_NEAR(tilt, 30.0f, 1e-3, "tilt clamped to +30");

    servo_tilt_set(-50.0f);
    servo_get_position(&pan, &tilt);
    CHECK_NEAR(tilt, 0.0f, 1e-3, "tilt clamped to 0");
}

static void test_servo_home_centers(void) {
    stub_reset();
    servo_init();
    servo_pan_set(45.0f);
    servo_tilt_set(20.0f);

    servo_home();

    float pan = -1.0f, tilt = -1.0f;
    servo_get_position(&pan, &tilt);
    CHECK_NEAR(pan,  0.0f, 1e-3, "home pan = 0");
    CHECK_NEAR(tilt, 0.0f, 1e-3, "home tilt = 0");
}

static void test_servo_set_limits_re_clamps(void) {
    stub_reset();
    servo_init();
    servo_pan_set(50.0f);
    servo_tilt_set(20.0f);

    servo_set_limits(-30.0f, 30.0f, 5.0f, 25.0f);

    float pan = -1.0f, tilt = -1.0f;
    servo_get_position(&pan, &tilt);
    CHECK_NEAR(pan,  30.0f, 1e-3, "pan re-clamped to new max");
    CHECK_NEAR(tilt, 20.0f, 1e-3, "tilt within new envelope kept");
}

/* =================================================================== */
/* BUTTON                                                               */
/* =================================================================== */

/* read_raw() returns digitalRead(PIN_BUTTON) == LOW (active-low pull-up).
 * stub_set_digital(pin, HIGH) -> released, LOW -> pressed. */

static void test_button_init_is_pullup(void) {
    stub_reset();
    stub_set_digital(PIN_BUTTON, HIGH); /* idle */
    button_init();
    CHECK_EQ(stub_get_mode(PIN_BUTTON), INPUT_PULLUP, "button mode");
    CHECK(button_process() == BUTTON_EVENT_NONE, "no event on init");
}

static void test_button_press_after_debounce(void) {
    stub_reset();
    stub_set_digital(PIN_BUTTON, HIGH);
    button_init();

    /* Pressed (LOW). */
    stub_set_digital(PIN_BUTTON, LOW);

    /* First tick: raw changed → no event yet. */
    uint8_t e = button_process();
    CHECK_EQ((int)e, BUTTON_EVENT_NONE, "no event before debounce window");

    /* Advance less than debounce: still no event. */
    stub_advance_millis(BUTTON_DEBOUNCE_MS - 10);
    e = button_process();
    CHECK_EQ((int)e, BUTTON_EVENT_NONE, "no event mid-debounce");

    /* Advance past debounce: PRESS exactly once. */
    stub_advance_millis(20);
    e = button_process();
    CHECK_EQ((int)e, BUTTON_EVENT_PRESS, "PRESS after debounce");

    /* Subsequent ticks while still pressed: no repeat events. */
    stub_advance_millis(50);
    e = button_process();
    CHECK_EQ((int)e, BUTTON_EVENT_NONE, "no repeat PRESS while held");
}

static void test_button_release_after_debounce(void) {
    stub_reset();
    stub_set_digital(PIN_BUTTON, LOW); /* start pressed */
    button_init();

    /* Release (HIGH). */
    stub_set_digital(PIN_BUTTON, HIGH);
    button_process();                       /* raw changed */
    stub_advance_millis(BUTTON_DEBOUNCE_MS + 5);
    uint8_t e = button_process();
    CHECK_EQ((int)e, BUTTON_EVENT_RELEASE, "RELEASE after debounce");
}

static void test_button_short_glitch_filtered(void) {
    stub_reset();
    stub_set_digital(PIN_BUTTON, HIGH);
    button_init();

    /* Brief glitch (5 ms): pressed then released before debounce window. */
    stub_set_digital(PIN_BUTTON, LOW);
    button_process();
    stub_advance_millis(5);
    stub_set_digital(PIN_BUTTON, HIGH);
    button_process();
    stub_advance_millis(BUTTON_DEBOUNCE_MS + 5);

    /* Should NOT have fired any event since stable level == initial. */
    uint8_t e = button_process();
    CHECK_EQ((int)e, BUTTON_EVENT_NONE, "glitch produces no event");
}

/* =================================================================== */
/* LASER                                                                */
/* =================================================================== */

static void test_laser_init_off_then_toggle(void) {
    stub_reset();
    laser_init();
    CHECK_EQ(stub_get_mode(PIN_LASER), OUTPUT, "laser mode");
    CHECK_EQ(stub_get_digital(PIN_LASER), LOW, "laser off after init");
    CHECK(!laser_is_on(), "laser_is_on false after init");

    laser_on();
    CHECK_EQ(stub_get_digital(PIN_LASER), HIGH, "laser on");
    CHECK(laser_is_on(), "laser_is_on true after on");

    laser_toggle();
    CHECK_EQ(stub_get_digital(PIN_LASER), LOW, "laser off after toggle");

    laser_toggle();
    CHECK_EQ(stub_get_digital(PIN_LASER), HIGH, "laser on after second toggle");

    laser_off();
    CHECK_EQ(stub_get_digital(PIN_LASER), LOW, "laser off");
}

/* =================================================================== */
/* HC-SR04                                                              */
/* =================================================================== */

static void test_hcsr04_init_configures_pins(void) {
    stub_reset();
    hcsr04_init();
    CHECK_EQ(stub_get_mode(PIN_HCSR04_1_TRIG), OUTPUT, "trig1 mode");
    CHECK_EQ(stub_get_mode(PIN_HCSR04_2_TRIG), OUTPUT, "trig2 mode");
    CHECK_EQ(stub_get_mode(PIN_HCSR04_1_ECHO), INPUT,  "echo1 mode");
    CHECK_EQ(stub_get_mode(PIN_HCSR04_2_ECHO), INPUT,  "echo2 mode");
    CHECK_EQ(stub_get_digital(PIN_HCSR04_1_TRIG), LOW, "trig1 idle low");
    CHECK_EQ(stub_get_digital(PIN_HCSR04_2_TRIG), LOW, "trig2 idle low");
    CHECK(!hcsr04_is_valid(0), "sensor 0 invalid at init");
    CHECK(!hcsr04_is_valid(1), "sensor 1 invalid at init");
}

static void test_hcsr04_trigger_pulses_high_then_low(void) {
    stub_reset();
    hcsr04_init();
    int initial_calls = stub_count_digital(PIN_HCSR04_1_TRIG);

    hcsr04_trigger(0);

    /* Three writes: LOW, HIGH, LOW. */
    int delta = stub_count_digital(PIN_HCSR04_1_TRIG) - initial_calls;
    CHECK_EQ(delta, 3, "trigger drives 3 levels");
    CHECK_EQ(stub_get_digital(PIN_HCSR04_1_TRIG), LOW, "trigger ends low");
}

static void test_hcsr04_invalid_id(void) {
    stub_reset();
    hcsr04_init();
    CHECK_NEAR(hcsr04_get_distance(2), -1.0f, 1e-6, "id=2 returns -1");
    CHECK(!hcsr04_is_valid(5), "id=5 is invalid");
}

static void test_hcsr04_distance_formula(void) {
    /* Compile-time sanity: 1000 us echo width =>
     *   d = 1000 us * 0.5 * 0.0343 cm/us = 17.15 cm */
    double d = 1000.0 * 0.5 * (double)HCSR04_SPEED_OF_SOUND_CM_US;
    CHECK_NEAR(d, 17.15, 0.05, "distance formula matches expected");
}

/* =================================================================== */

int main(void) {
    test_motor_init_drives_zero_and_enables();
    test_motor_set_forward();
    test_motor_set_reverse();
    test_motor_set_clamps_oversaturated_inputs();
    test_motor_emergency_stop();
    test_motor_disable_drops_en_and_ignores_set();

    test_servo_pan_clamps_to_limits();
    test_servo_tilt_clamps_to_limits();
    test_servo_home_centers();
    test_servo_set_limits_re_clamps();

    test_button_init_is_pullup();
    test_button_press_after_debounce();
    test_button_release_after_debounce();
    test_button_short_glitch_filtered();

    test_laser_init_off_then_toggle();

    test_hcsr04_init_configures_pins();
    test_hcsr04_trigger_pulses_high_then_low();
    test_hcsr04_invalid_id();
    test_hcsr04_distance_formula();

    std::printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
