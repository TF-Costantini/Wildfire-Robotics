/**
 * motor_tb6612.c — Wildfire Robotics UGV
 * TODO: Implementare driver per motor driver TB6612FNG
 *
 * Il TB6612FNG è un dual motor driver che controlla 2 DC motor
 * con segnali: AIN1/AIN2 per direzione Motor A, BIN1/BIN2 per Motor B,
 * PWMA/PWMB per velocità, STBY per standby.
 *
 * Logica direzione:
 *   AIN1=1, AIN2=0 → CW
 *   AIN1=0, AIN2=1 → CCW
 *   AIN1=0, AIN2=0 → Coast (fermo)
 *   STBY=0 → Standby (tutti i motori fermi)
 */

#include "motor_tb6612.h"
#include "pins.h"
#include <stdint.h>

// === STATI INTERNI ===
static struct {
    int16_t speed_left;    // -100 to 100
    int16_t speed_right;   // -100 to 100
    uint8_t standby;       // 0=standby, 1=active
} motor_state = {0, 0, 0};

// === HELPER FUNCTIONS ===

/**
 * TODO: Impostare direzione Motor A
 * @param dir: 1=CW, 0=Coast, -1=CCW
 */
static void motor_a_set_direction(int8_t dir) {
    // if (dir > 0) {
    //     // CW
    //     HAL_GPIO_WritePin(MOTOR_PORT, PIN_MOTOR_AIN1, GPIO_PIN_SET);
    //     HAL_GPIO_WritePin(MOTOR_PORT, PIN_MOTOR_AIN2, GPIO_PIN_RESET);
    // } else if (dir < 0) {
    //     // CCW
    //     HAL_GPIO_WritePin(MOTOR_PORT, PIN_MOTOR_AIN1, GPIO_PIN_RESET);
    //     HAL_GPIO_WritePin(MOTOR_PORT, PIN_MOTOR_AIN2, GPIO_PIN_SET);
    // } else {
    //     // Coast
    //     HAL_GPIO_WritePin(MOTOR_PORT, PIN_MOTOR_AIN1, GPIO_PIN_RESET);
    //     HAL_GPIO_WritePin(MOTOR_PORT, PIN_MOTOR_AIN2, GPIO_PIN_RESET);
    // }
}

/**
 * TODO: Impostare direzione Motor B
 * @param dir: 1=CW, 0=Coast, -1=CCW
 */
static void motor_b_set_direction(int8_t dir) {
    // Similar to motor_a_set_direction
}

/**
 * TODO: Impostare velocità Motor A (PWM)
 * @param speed: -100 to 100 (signed)
 */
static void motor_a_set_speed(int16_t speed) {
    // Clamp speed
    // speed = (speed > 100) ? 100 : (speed < -100) ? -100 : speed;
    //
    // if (speed >= 0) {
    //     motor_a_set_direction(1);
    //     __HAL_TIM_SET_COMPARE(&htim_motor, TIM_CHANNEL_1, speed);
    // } else {
    //     motor_a_set_direction(-1);
    //     __HAL_TIM_SET_COMPARE(&htim_motor, TIM_CHANNEL_1, -speed);
    // }
}

/**
 * TODO: Impostare velocità Motor B (PWM)
 * @param speed: -100 to 100 (signed)
 */
static void motor_b_set_speed(int16_t speed) {
    // Similar to motor_a_set_speed
}

/**
 * TODO: Abilitare/disabilitare standby
 * @param enable: 1=active, 0=standby
 */
static void motor_set_standby(uint8_t enable) {
    // HAL_GPIO_WritePin(MOTOR_PORT, PIN_MOTOR_STBY, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// === API PUBBLICA ===

/**
 * TODO: Inizializzare il driver motori
 * Configurare GPIO, PWM timer, mettere in standby
 */
void motor_init(void) {
    // TODO: Abilitare clock GPIO per motori
    // TODO: Configurare GPIO come output
    // TODO: Inizializzare PWM timer per PWMA e PWMB
    // TODO: Inizialmente in standby
    motor_state.standby = 0;
    motor_state.speed_left = 0;
    motor_state.speed_right = 0;
}

/**
 * TODO: Impostare velocità per entrambi i motori
 * @param left: -100 to 100 (negativo = indietro)
 * @param right: -100 to 100 (negativo = indietro)
 */
void motor_set_speed(int16_t left, int16_t right) {
    // Clamp values
    // left = (left > 100) ? 100 : (left < -100) ? -100 : left;
    // right = (right > 100) ? 100 : (right < -100) ? -100 : right;

    motor_state.speed_left = left;
    motor_state.speed_right = right;

    // Apply if not in standby
    if (motor_state.standby) {
        motor_a_set_speed(left);
        motor_b_set_speed(right);
    }
}

/**
 * TODO: Arresto immediato di emergenza
 * Imposta entrambi i motori a 0
 */
void motor_emergency_stop(void) {
    motor_state.speed_left = 0;
    motor_state.speed_right = 0;
    motor_a_set_speed(0);
    motor_b_set_speed(0);
}

/**
 * TODO: Abilitare motori (uscita da standby)
 */
void motor_enable(void) {
    motor_state.standby = 1;
    motor_set_standby(1);
}

/**
 * TODO: Disabilitare motori (entrata in standby)
 */
void motor_disable(void) {
    motor_state.standby = 0;
    motor_set_standby(0);
    motor_emergency_stop();
}

/**
 * TODO: Ottenere stato attuale motori
 * @param left: output per velocità sinistra
 * @param right: output per velocità destra
 */
void motor_get_state(int16_t *left, int16_t *right) {
    if (left) *left = motor_state.speed_left;
    if (right) *right = motor_state.speed_right;
}
