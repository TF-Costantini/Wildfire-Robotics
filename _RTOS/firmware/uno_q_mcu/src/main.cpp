/**
 * main.cpp — Wildfire Robotics UGV
 *
 * micro-ROS node for the STM32U585 side of the Arduino UNO Q.
 * Bridges the real-time I/O (motors, servos, ultrasonics, laser,
 * button) to the ROS2 graph that runs on the MPU side.
 *
 * Topics
 * ------
 *  Publishers
 *    /ultrasonic/left   sensor_msgs/Range          @ 10 Hz
 *    /ultrasonic/right  sensor_msgs/Range          @ 10 Hz
 *    /button_event      wildfire_msgs/ButtonEvent  on press only
 *
 *  Subscribers (these are the GATED commands from state_machine_node;
 *  the raw /cmd_drive, /cmd_pantilt, /cmd_laser produced by the
 *  controllers are republished as /cmd_*_mcu only when the active
 *  state allows it.)
 *    /cmd_drive_mcu     wildfire_msgs/DriveCmd     -> motor_set()
 *    /cmd_pantilt_mcu   wildfire_msgs/PanTiltCmd   -> servo_set_position()
 *    /cmd_laser_mcu     std_msgs/Bool              -> laser_on()/off()
 *
 * Safety
 * ------
 *  - Drive watchdog: if no /cmd_drive arrives within
 *    WATCHDOG_TIMEOUT_MS, motors are forced to (0, 0).
 *  - On agent loss the node is destroyed and recreated; while
 *    disconnected the watchdog keeps motors stopped.
 *  - HC-SR04 ECHO is captured via EXTI inside the driver, so
 *    sensor reads never block the executor.
 */

#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <sensor_msgs/msg/range.h>
#include <std_msgs/msg/bool.h>
#include <wildfire_msgs/msg/button_event.h>
#include <wildfire_msgs/msg/drive_cmd.h>
#include <wildfire_msgs/msg/pan_tilt_cmd.h>

extern "C" {
#include "drivers/motor_bts7960.h"
#include "drivers/servo_mg996r.h"
#include "drivers/ultrasonic_hcsr04.h"
#include "drivers/laser_ky008.h"
#include "drivers/button.h"
}

#include "pins.h"

// =====================================================================
// Constants
// =====================================================================
#define NODE_NAME              "uno_q_mcu_node"
#define ROS_NAMESPACE          ""

#define ULTRASONIC_PERIOD_MS   100      // 10 Hz publish rate
#define HCSR04_TRIG_PERIOD_MS  60       // trigger faster than publish
#define WATCHDOG_TIMEOUT_MS    500

#define EXECUTOR_HANDLES       3        // 3 subscribers

#define HCSR04_FOV_RAD         0.26f    // ~15 deg cone
#define HCSR04_MIN_RANGE_M     0.02f
#define HCSR04_MAX_RANGE_M     4.00f

#define MICRO_ROS_AGENT_PING_MS  500
#define MICRO_ROS_AGENT_PING_TO  100

// =====================================================================
// Lifecycle state machine (mirrors the micro-ROS Arduino examples).
// =====================================================================
enum AgentState {
    WAITING_AGENT,
    AGENT_AVAILABLE,
    AGENT_CONNECTED,
    AGENT_DISCONNECTED,
};

static AgentState g_agent_state = WAITING_AGENT;

// =====================================================================
// micro-ROS objects
// =====================================================================
static rcl_allocator_t g_allocator;
static rclc_support_t  g_support;
static rcl_node_t      g_node;
static rclc_executor_t g_executor;

static rcl_publisher_t g_pub_us_left;
static rcl_publisher_t g_pub_us_right;
static rcl_publisher_t g_pub_button;

static rcl_subscription_t g_sub_drive;
static rcl_subscription_t g_sub_pantilt;
static rcl_subscription_t g_sub_laser;

// Reusable message buffers
static sensor_msgs__msg__Range          g_msg_us_left;
static sensor_msgs__msg__Range          g_msg_us_right;
static wildfire_msgs__msg__ButtonEvent  g_msg_button;
static wildfire_msgs__msg__DriveCmd     g_msg_drive;
static wildfire_msgs__msg__PanTiltCmd   g_msg_pantilt;
static std_msgs__msg__Bool              g_msg_laser;

// =====================================================================
// Bookkeeping
// =====================================================================
static uint32_t g_last_drive_cmd_ms = 0;
static uint32_t g_last_us_publish_ms = 0;
static uint32_t g_last_us_trigger_ms = 0;
static uint32_t g_last_agent_check_ms = 0;
static bool     g_drive_zeroed_by_wd  = false;

// =====================================================================
// Helpers
// =====================================================================
static void error_blink(void) {
    pinMode(LED_BUILTIN, OUTPUT);
    while (true) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        delay(120);
    }
}

#define RCCHECK(fn)  do { rcl_ret_t _rc = (fn); if (_rc != RCL_RET_OK) { error_blink(); } } while (0)
#define RCSOFT(fn)   do { rcl_ret_t _rc_ignored = (fn); (void)_rc_ignored; } while (0)

static inline void fill_time_now(builtin_interfaces__msg__Time *t) {
    uint32_t ms = millis();
    t->sec     = (int32_t)(ms / 1000);
    t->nanosec = (uint32_t)((ms % 1000) * 1000000UL);
}

static void init_range_msg(sensor_msgs__msg__Range *m, const char *frame_id) {
    m->radiation_type = sensor_msgs__msg__Range__ULTRASOUND;
    m->field_of_view  = HCSR04_FOV_RAD;
    m->min_range      = HCSR04_MIN_RANGE_M;
    m->max_range      = HCSR04_MAX_RANGE_M;
    m->range          = 0.0f;
    m->header.frame_id.data     = (char *)frame_id;
    m->header.frame_id.size     = strlen(frame_id);
    m->header.frame_id.capacity = m->header.frame_id.size + 1;
}

// =====================================================================
// Subscription callbacks
// =====================================================================
static void cb_drive(const void *msgin) {
    const wildfire_msgs__msg__DriveCmd *m =
        (const wildfire_msgs__msg__DriveCmd *)msgin;
    motor_set(m->left, m->right);
    g_last_drive_cmd_ms = millis();
    g_drive_zeroed_by_wd = false;
}

static void cb_pantilt(const void *msgin) {
    const wildfire_msgs__msg__PanTiltCmd *m =
        (const wildfire_msgs__msg__PanTiltCmd *)msgin;
    servo_set_position(m->pan_deg, m->tilt_deg);
}

static void cb_laser(const void *msgin) {
    const std_msgs__msg__Bool *m = (const std_msgs__msg__Bool *)msgin;
    if (m->data) laser_on();
    else         laser_off();
}

// =====================================================================
// micro-ROS entity create / destroy
// =====================================================================
static bool create_entities(void) {
    g_allocator = rcl_get_default_allocator();

    if (rclc_support_init(&g_support, 0, NULL, &g_allocator) != RCL_RET_OK)
        return false;

    if (rclc_node_init_default(&g_node, NODE_NAME, ROS_NAMESPACE, &g_support)
        != RCL_RET_OK)
        return false;

    // --- Publishers ----------------------------------------------------
    RCCHECK(rclc_publisher_init_default(
        &g_pub_us_left, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range),
        "/ultrasonic/left"));

    RCCHECK(rclc_publisher_init_default(
        &g_pub_us_right, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range),
        "/ultrasonic/right"));

    RCCHECK(rclc_publisher_init_default(
        &g_pub_button, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(wildfire_msgs, msg, ButtonEvent),
        "/button_event"));

    // --- Subscribers ---------------------------------------------------
    RCCHECK(rclc_subscription_init_default(
        &g_sub_drive, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(wildfire_msgs, msg, DriveCmd),
        "/cmd_drive_mcu"));

    RCCHECK(rclc_subscription_init_default(
        &g_sub_pantilt, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(wildfire_msgs, msg, PanTiltCmd),
        "/cmd_pantilt_mcu"));

    RCCHECK(rclc_subscription_init_default(
        &g_sub_laser, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
        "/cmd_laser_mcu"));

    // --- Executor ------------------------------------------------------
    RCCHECK(rclc_executor_init(&g_executor, &g_support.context,
                               EXECUTOR_HANDLES, &g_allocator));

    RCCHECK(rclc_executor_add_subscription(
        &g_executor, &g_sub_drive,   &g_msg_drive,
        &cb_drive,   ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(
        &g_executor, &g_sub_pantilt, &g_msg_pantilt,
        &cb_pantilt, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(
        &g_executor, &g_sub_laser,   &g_msg_laser,
        &cb_laser,   ON_NEW_DATA));

    // --- Persistent message setup -------------------------------------
    init_range_msg(&g_msg_us_left,  "ultrasonic_left");
    init_range_msg(&g_msg_us_right, "ultrasonic_right");
    g_msg_button.kind = 0; // PRESS

    return true;
}

static void destroy_entities(void) {
    rmw_context_t *rmw_ctx = rcl_context_get_rmw_context(&g_support.context);
    (void)rmw_uros_set_context_entity_destroy_session_timeout(rmw_ctx, 0);

    RCSOFT(rcl_publisher_fini(&g_pub_us_left,  &g_node));
    RCSOFT(rcl_publisher_fini(&g_pub_us_right, &g_node));
    RCSOFT(rcl_publisher_fini(&g_pub_button,   &g_node));
    RCSOFT(rcl_subscription_fini(&g_sub_drive,   &g_node));
    RCSOFT(rcl_subscription_fini(&g_sub_pantilt, &g_node));
    RCSOFT(rcl_subscription_fini(&g_sub_laser,   &g_node));
    rclc_executor_fini(&g_executor);
    RCSOFT(rcl_node_fini(&g_node));
    rclc_support_fini(&g_support);
}

// =====================================================================
// Periodic tasks (called only when AGENT_CONNECTED)
// =====================================================================
static void task_publish_ultrasonics(void) {
    const uint32_t now = millis();

    if ((uint32_t)(now - g_last_us_trigger_ms) >= HCSR04_TRIG_PERIOD_MS) {
        g_last_us_trigger_ms = now;
        hcsr04_trigger_all();
    }

    if ((uint32_t)(now - g_last_us_publish_ms) >= ULTRASONIC_PERIOD_MS) {
        g_last_us_publish_ms = now;

        fill_time_now(&g_msg_us_left.header.stamp);
        fill_time_now(&g_msg_us_right.header.stamp);

        float dl = hcsr04_get_distance(HCSR04_LEFT);
        float dr = hcsr04_get_distance(HCSR04_RIGHT);

        // Publish meters if valid, else NaN. ROS Range convention:
        // values outside [min_range, max_range] are not valid.
        g_msg_us_left.range  = (dl >= 0.0f) ? (dl * 0.01f) : NAN;
        g_msg_us_right.range = (dr >= 0.0f) ? (dr * 0.01f) : NAN;

        RCSOFT(rcl_publish(&g_pub_us_left,  &g_msg_us_left,  NULL));
        RCSOFT(rcl_publish(&g_pub_us_right, &g_msg_us_right, NULL));
    }
}

static void task_publish_button(void) {
    uint8_t ev = button_process();
    if (ev == BUTTON_EVENT_PRESS) {
        fill_time_now(&g_msg_button.stamp);
        g_msg_button.kind = 0; // PRESS
        RCSOFT(rcl_publish(&g_pub_button, &g_msg_button, NULL));
    }
}

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
    Serial.begin(115200);
    set_microros_serial_transports(Serial);

    motor_init();
    servo_init();
    hcsr04_init();
    laser_init();
    button_init();

    motor_set(0.0f, 0.0f);
    servo_home();
    laser_off();

    g_last_drive_cmd_ms   = millis();
    g_last_us_publish_ms  = millis();
    g_last_us_trigger_ms  = millis();
    g_last_agent_check_ms = millis();
    g_agent_state         = WAITING_AGENT;
}

void loop(void) {
    const uint32_t now = millis();

    // --- Sensor housekeeping (always-on, agent-independent) -----------
    hcsr04_update();
    task_drive_watchdog();

    // --- Agent lifecycle ---------------------------------------------
    if ((uint32_t)(now - g_last_agent_check_ms) >= MICRO_ROS_AGENT_PING_MS) {
        g_last_agent_check_ms = now;

        switch (g_agent_state) {
        case WAITING_AGENT:
            g_agent_state =
                (rmw_uros_ping_agent(MICRO_ROS_AGENT_PING_TO, 1) == RCL_RET_OK)
                    ? AGENT_AVAILABLE : WAITING_AGENT;
            break;

        case AGENT_AVAILABLE:
            g_agent_state = create_entities() ? AGENT_CONNECTED
                                              : WAITING_AGENT;
            if (g_agent_state == WAITING_AGENT) destroy_entities();
            break;

        case AGENT_CONNECTED:
            g_agent_state =
                (rmw_uros_ping_agent(MICRO_ROS_AGENT_PING_TO, 1) == RCL_RET_OK)
                    ? AGENT_CONNECTED : AGENT_DISCONNECTED;
            break;

        case AGENT_DISCONNECTED:
            destroy_entities();
            motor_set(0.0f, 0.0f);
            laser_off();
            g_agent_state = WAITING_AGENT;
            break;
        }
    }

    // --- Active work only while connected -----------------------------
    if (g_agent_state == AGENT_CONNECTED) {
        task_publish_ultrasonics();
        task_publish_button();
        rclc_executor_spin_some(&g_executor, RCL_MS_TO_NS(5));
    }
}