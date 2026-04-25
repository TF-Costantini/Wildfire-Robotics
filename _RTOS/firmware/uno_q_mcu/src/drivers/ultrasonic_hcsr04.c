/**
 * ultrasonic_hcsr04.c — Wildfire Robotics UGV
 * TODO: Implementare driver per sensori HC-SR04
 *
 * HC-SR04 usa ultrasuoni per misurare distanza:
 * - TRIG: invia burst ultrasound (10μs pulse)
 * - ECHO: riceve eco, larghezza impulso = distanza
 * - Formula: distance = (echo_width_us / 2) * speed_of_sound
 *
 * Due sensori: front-left (#1) e front-right (#2)
 */

#include "ultrasonic_hcsr04.h"
#include "pins.h"
#include <stdint.h>
#include <stdbool.h>

// === STATI ===
typedef struct {
    volatile uint8_t state;          // 0=idle, 1=trig_high, 2=waiting_echo
    volatile uint32_t echo_start;     // timestamp start (μs)
    volatile uint32_t echo_end;      // timestamp end (μs)
    volatile bool valid;             // true se misura valida
    float distance_cm;                // ultima distanza valida
} hcsr04_sensor_t;

static hcsr04_sensor_t sensor[2] = {
    {.state = 0, .echo_start = 0, .echo_end = 0, .valid = false, .distance_cm = 0},
    {.state = 0, .echo_start = 0, .echo_end = 0, .valid = false, .distance_cm = 0}
};

// Timestamp per trigger timing
static uint32_t get_tick_us(void) {
    // TODO: Implementare gettick usando DWT->CYCCNT o similar
    // return DWT->CYCCNT / SystemCoreClock * 1000000;
    return 0;
}

// === API PUBBLICA ===

/**
 * TODO: Inizializzare i sensori HC-SR04
 * Configurare GPIO: TRIG come output, ECHO come input con interrupt
 */
void hcsr04_init(void) {
    // TODO: Abilitare clock GPIO
    // TODO: Configurare TRIG come output push-pull
    // TODO: Configurare ECHO come input
    // TODO: Configurare interrupt su ECHO (rising + falling edge)

    // Reset state
    for (int i = 0; i < 2; i++) {
        sensor[i].state = 0;
        sensor[i].valid = false;
        sensor[i].distance_cm = 0;
    }
}

/**
 * TODO: Avviare una misura sul sensore specificato
 * @param id: 0=sensor#1 (left), 1=sensor#2 (right)
 */
void hcsr04_trigger(uint8_t id) {
    if (id > 1) return;

    // Ensure TRIG is low
    // HAL_GPIO_WritePin(HCSR04_PORT, sensor[id].trig_pin, GPIO_PIN_RESET);

    // Wait 2μs
    // delay_us(2);

    // TRIG high for 10μs
    // HAL_GPIO_WritePin(HCSR04_PORT, sensor[id].trig_pin, GPIO_PIN_SET);
    // delay_us(10);
    // HAL_GPIO_WritePin(HCSR04_PORT, sensor[id].trig_pin, GPIO_PIN_RESET);

    sensor[id].state = 1;  // trig sent
}

/**
 * TODO: Avviare misura su entrambi i sensori
 */
void hcsr04_trigger_all(void) {
    hcsr04_trigger(0);
    // Small delay between triggers to avoid crosstalk
    // delay_us(100);
    hcsr04_trigger(1);
}

/**
 * TODO: Gestire interrupt ECHO - front-left
 * Su rising edge: registrare timestamp start
 * Su falling edge: calcolare distanza
 */
void hcsr04_echo_interrupt_left(void) {
    // if (sensor[0].state == 1) {
    //     // Rising edge - start timing
    //     sensor[0].echo_start = get_tick_us();
    //     sensor[0].state = 2;
    // } else if (sensor[0].state == 2) {
    //     // Falling edge - stop timing
    //     sensor[0].echo_end = get_tick_us();
    //     sensor[0].state = 0;
    //
    //     // Calculate distance
    //     uint32_t echo_width = sensor[0].echo_end - sensor[0].echo_start;
    //
    //     // Check timeout (max ~4.3m)
    //     if (echo_width < HCSR04_TIMEOUT_US) {
    //         sensor[0].distance_cm = (echo_width / 2.0) * HCSR04_SPEED_OF_SOUND_CM_US;
    //         sensor[0].valid = true;
    //     } else {
    //         sensor[0].valid = false;
    //     }
    // }
}

/**
 * TODO: Gestire interrupt ECHO - front-right
 */
void hcsr04_echo_interrupt_right(void) {
    // Similar to left
}

/**
 * TODO: Ottenere distanza letta
 * @param id: 0=left, 1=right
 * @return: distanza in cm, -1 se non valida
 */
float hcsr04_get_distance(uint8_t id) {
    if (id > 1) return -1.0;
    if (!sensor[id].valid) return -1.0;
    return sensor[id].distance_cm;
}

/**
 * TODO: Ottenere distanza con timeout
 * @param id: 0=left, 1=right
 * @param timeout_ms: timeout in millisecondi
 * @return: distanza in cm, -1 se timeout
 */
float hcsr04_get_distance_timeout(uint8_t id, uint32_t timeout_ms) {
    // Start measurement
    hcsr04_trigger(id);

    // Wait for valid reading or timeout
    // uint32_t start = HAL_GetTick();
    // while (!sensor[id].valid && (HAL_GetTick() - start) < timeout_ms) {
    //     // Could add small delay here
    // }

    return hcsr04_get_distance(id);
}

/**
 * TODO: Verificare se la misura è valida
 * @param id: 0=left, 1=right
 * @return: true se distanza valida
 */
bool hcsr04_is_valid(uint8_t id) {
    if (id > 1) return false;
    return sensor[id].valid;
}
