/**
 * motor_tb6612.h — Wildfire Robotics UGV
 */

#ifndef MOTOR_TB6612_H
#define MOTOR_TB6612_H

#include <stdint.h>

// === API PUBBLICA ===

/**
 * Inizializzare il driver motori TB6612FNG
 */
void motor_init(void);

/**
 * Impostare velocità motori
 * @param left: -100 to 100 (negativo = indietro)
 * @param right: -100 to 100 (negativo = indietro)
 */
void motor_set_speed(int16_t left, int16_t right);

/**
 * Arresto immediato di emergenza
 */
void motor_emergency_stop(void);

/**
 * Abilitare motori (uscita da standby)
 */
void motor_enable(void);

/**
 * Disabilitare motori (entrata in standby)
 */
void motor_disable(void);

/**
 * Ottenere stato attuale motori
 * @param left: output per velocità sinistra (può essere NULL)
 * @param right: output per velocità destra (può essere NULL)
 */
void motor_get_state(int16_t *left, int16_t *right);

#endif // MOTOR_TB6612_H
