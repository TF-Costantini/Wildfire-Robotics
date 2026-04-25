/**
 * main.c — Wildfire Robotics UGV
 * Firmware principale per MCU STM32U585 (Arduino Uno Q)
 * TODO: Implementare il main loop del firmware
 *
 * Responsabilità:
 * - Inizializzare tutti i driver (motori, servo, HC-SR04, laser, button)
 * - Gestire la comunicazione micro-ROS
 * - Implementare il watchdog per i comandi
 * - Pubblicare dati ultrasonic e button_event
 * - Ricevere e applicare comandi drive, pantilt, laser
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Include drivers
#include "drivers/motor_tb6612.h"
#include "drivers/servo_mg996r.h"
#include "drivers/ultrasonic_hcsr04.h"
#include "drivers/laser_ky008.h"
#include "drivers/button.h"

// Include micro-ROS (da implementare)
// #include <microros/msgs.h>

// === COSTANTI ===
#define ULTRASONIC_POLL_RATE_MS    100    // 10Hz per lettura ultrasonica
#define WATCHDOG_TIMEOUT_MS         500    // 500ms watchdog
#define MAIN_LOOP_RATE_HZ           10     // 10Hz main loop

// === STATO GLOBALE ===
typedef struct {
    // Stato sistema
    uint8_t mode;  // 0=IDLE, 1=FOLLOW, 2=FIRE

    // Watchdog
    uint32_t last_cmd_time_ms;

    // Flag errori
    uint8_t error_flags;
} system_state_t;

static system_state_t sys_state = {
    .mode = 0,
    .last_cmd_time_ms = 0,
    .error_flags = 0
};

// === PROTOTIPI ===
static void system_init(void);
static void main_loop(void);
static void update_ultrasonic(void);
static void update_watchdog(void);
static void handle_button_events(void);
static void apply_commands_from_ros(void);
static void publish_ultrasonic_data(void);
static void publish_button_event(uint8_t event);

// === MAIN ===
/**
 * TODO: Entry point principale
 */
int main(void) {
    // TODO: System initialization
    // SystemClock_Config();
    // MX_GPIO_Init();
    // MX_TIM_Init();

    system_init();

    printf("[Wildfire UGV] Firmware avviato!\n");

    // TODO: Inizializzare transport micro-ROS
    // rmuros_transport_init();

    // === MAIN LOOP ===
    while (1) {
        // TODO: Spin micro-ROS (se disponibile)
        // rmuros_spin_some();

        main_loop();

        // TODO: Delay o RTOS delay
        // HAL_Delay(1000 / MAIN_LOOP_RATE_HZ);
    }

    return 0;
}

// === INIZIALIZZAZIONE ===
/**
 * TODO: Inizializzare tutti i sottosistemi
 */
static void system_init(void) {
    printf("[Init] Inizializzazione driver...\n");

    // Inizializzare driver nell'ordine
    motor_init();
    printf("[Init] Motor driver OK\n");

    servo_init();
    printf("[Init] Servo OK\n");

    hcsr04_init();
    printf("[Init] HC-SR04 OK\n");

    laser_init();
    printf("[Init] Laser OK\n");

    button_init();
    printf("[Init] Button OK\n");

    // Stato iniziale
    sys_state.mode = 0;  // IDLE
    sys_state.last_cmd_time_ms = 0;
    sys_state.error_flags = 0;

    // Posizione home per servo
    servo_home();

    // Laser spento
    laser_off();

    // Motori fermi
    motor_set_speed(0, 0);

    printf("[Init] Sistema pronto!\n");
}

// === MAIN LOOP ===
/**
 * TODO: Implementare main loop
 */
static void main_loop(void) {
    uint32_t now_ms = 0;  // TODO: usare HAL_GetTick()

    // 1. Gestire eventi pulsante
    handle_button_events();

    // 2. Aggiornare sensori ultrasonici (polling)
    update_ultrasonic();

    // 3. Controllare watchdog
    update_watchdog();

    // 4. Applicare comandi da ROS (se disponibili)
    apply_commands_from_ros();

    // 5. Pubblicare dati su ROS (se micro-ROS attivo)
    // publish_ultrasonic_data();
}

/**
 * TODO: Gestire eventi del pulsante
 * Genera transizioni di stato: IDLE→FOLLOW→FIRE→IDLE
 */
static void handle_button_events(void) {
    uint8_t event = button_process();

    if (event == BUTTON_EVENT_PRESS) {
        printf("[Button] Press detected\n");
        publish_button_event(event);

        // Rotazione stato
        if (sys_state.mode == 0) {
            sys_state.mode = 1;  // IDLE → FOLLOW
            printf("[State] → FOLLOW\n");
        } else if (sys_state.mode == 1) {
            sys_state.mode = 2;  // FOLLOW → FIRE
            printf("[State] → FIRE\n");
        } else {
            sys_state.mode = 0;  // FIRE → IDLE
            printf("[State] → IDLE\n");
        }

        // TODO: Pubblicare nuovo stato su /mode
    }
}

/**
 * TODO: Aggiornare letture HC-SR04 (polling periodico)
 */
static void update_ultrasonic(void) {
    static uint32_t last_update_ms = 0;
    uint32_t now_ms = 0;  // TODO: usare HAL_GetTick()

    if ((now_ms - last_update_ms) >= ULTRASONIC_POLL_RATE_MS) {
        last_update_ms = now_ms;

        // Trigger nuova misura
        hcsr04_trigger_all();
    }
}

/**
 * TODO: Implementare watchdog
 * Se non arrivano comandi entro timeout, fermare tutto
 */
static void update_watchdog(void) {
    uint32_t now_ms = 0;  // TODO: usare HAL_GetTick()

    if ((now_ms - sys_state.last_cmd_time_ms) > WATCHDOG_TIMEOUT_MS) {
        // Watchdog scaduto - emergency stop
        if (sys_state.last_cmd_time_ms > 0) {  // Non eseguire all'avvio
            printf("[Watchdog] Timeout! Emergency stop!\n");
            motor_set_speed(0, 0);
            laser_off();
            // TODO: Pubblicare warning
        }
        sys_state.last_cmd_time_ms = now_ms;  // Reset
    }
}

/**
 * TODO: Ricevere comandi da micro-ROS e applicarli
 * I comandi vengono applicati solo se coerenti con lo stato corrente
 */
static void apply_commands_from_ros(void) {
    // TODO: Leggere comandi da subscriber micro-ROS:
    // - /cmd_drive_mcu (DriveCmd)
    // - /cmd_pantilt_mcu (PanTiltCmd)
    // - /cmd_laser_mcu (Bool)
    //
    // Logica di gating:
    // - IDLE: tutto a zero
    // - FOLLOW: solo drive attivo, pantilt→home, laser OFF
    // - FIRE: solo pantilt e laser, drive a zero
    //
    // Aggiornare last_cmd_time_ms ad ogni comando ricevuto

    // Placeholder: comandi zero in IDLE
    if (sys_state.mode == 0) {
        // IDLE: fermare tutto
        // motor_set_speed(0, 0);
        // servo_home();
        // laser_off();
    }
}

/**
 * TODO: Pubblicare dati ultrasonic su ROS
 */
static void publish_ultrasonic_data(void) {
    // TODO: Se micro-ROS disponibile
    // float dist_left = hcsr04_get_distance(0);
    // float dist_right = hcsr04_get_distance(1);
    //
    // Pubblicare su /ultrasonic/left e /ultrasonic/right
    // come sensor_msgs/Range
}

/**
 * TODO: Pubblicare evento pulsante su ROS
 */
static void publish_button_event(uint8_t event) {
    // TODO: Se micro-ROS disponibile
    // Pubblicare su /button_event
    // wildfire_msgs/ButtonEvent
}

// === HANDLER INTERRUZIONI ===
/**
 * TODO: EXTI Handler per ECHO HC-SR04
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    // if (GPIO_Pin == PIN_HCSR04_1_ECHO) {
    //     hcsr04_echo_interrupt_left();
    // } else if (GPIO_Pin == PIN_HCSR04_2_ECHO) {
    //     hcsr04_echo_interrupt_right();
    // }
}

/**
 * TODO: Timer callback per PWM servo
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    // Per aggiornare PWM servo
}

/**
 * TODO: Error callback
 */
void Error_Handler(void) {
    printf("[ERROR] Errore di sistema!\n");
    sys_state.error_flags |= 0x01;
    while (1) {
        // Blink LED di errore
    }
}
