/**
 * ultrasonic_hcsr04.h — Wildfire Robotics UGV
 */

#ifndef ULTRASONIC_HCSR04_H
#define ULTRASONIC_HCSR04_H

#include <stdint.h>
#include <stdbool.h>

// === API PUBBLICA ===

/**
 * Inizializzare i sensori HC-SR04
 */
void hcsr04_init(void);

/**
 * Avviare una misura sul sensore specificato
 * @param id: 0=sensor#1 (left), 1=sensor#2 (right)
 */
void hcsr04_trigger(uint8_t id);

/**
 * Avviare misura su entrambi i sensori
 */
void hcsr04_trigger_all(void);

/**
 * Ottenere distanza letta
 * @param id: 0=left, 1=right
 * @return: distanza in cm, -1 se non valida
 */
float hcsr04_get_distance(uint8_t id);

/**
 * Ottenere distanza con timeout
 * @param id: 0=left, 1=right
 * @param timeout_ms: timeout in millisecondi
 * @return: distanza in cm, -1 se timeout
 */
float hcsr04_get_distance_timeout(uint8_t id, uint32_t timeout_ms);

/**
 * Verificare se la misura è valida
 * @param id: 0=left, 1=right
 * @return: true se distanza valida
 */
bool hcsr04_is_valid(uint8_t id);

/**
 * Gestire interrupt ECHO per sensore sinistro
 */
void hcsr04_echo_interrupt_left(void);

/**
 * Gestire interrupt ECHO per sensore destro
 */
void hcsr04_echo_interrupt_right(void);

#endif // ULTRASONIC_HCSR04_H
