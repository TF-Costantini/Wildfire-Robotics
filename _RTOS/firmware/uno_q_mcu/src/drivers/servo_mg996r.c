/**
 * servo_mg996r.c — Wildfire Robotics UGV
 * TODO: Implementare driver per servo MG996R
 *
 * Il MG996R è un servo ad alta coppia che richiede:
 * - PWM a 50Hz (periodo 20ms)
 * - Duty cycle da 1ms (0°) a 2ms (180°)
 * - Attenzione: alcuni servo hanno offset e range limitato
 *
 * Il driver supporta 2 canali: PAN e TILT
 */

#include "servo_mg996r.h"
#include "pins.h"
#include <stdint.h>

// === COSTANTI CALIBRAZIONE ===
// TODO: Calibrare questi valori per i vostri specifici servo
#define SERVO_PAN_OFFSET_DEG     0.0    // Offset in gradi
#define SERVO_TILT_OFFSET_DEG     0.0    // Offset in gradi

// === STATI INTERNI ===
static struct {
    float pan_deg;     // Posizione attuale pan in gradi
    float tilt_deg;    // Posizione attuale tilt in gradi
    float pan_min;
    float pan_max;
    float tilt_min;
    float tilt_max;
} servo_state = {
    .pan_deg = 0.0,
    .tilt_deg = 0.0,
    .pan_min = -60.0,
    .pan_max = 60.0,
    .tilt_min = 0.0,
    .tilt_max = 30.0
};

// === API PUBBLICA ===

/**
 * TODO: Inizializzare i servo
 * Configurare PWM timer a 50Hz per entrambi i canali
 * Posizionare i servo alla posizione home (0°, 0°)
 */
void servo_init(void) {
    // TODO: Abilitare clock GPIO per pin servo
    // TODO: Configurare GPIO come AF (PWM)
    // TODO: Inizializzare PWM timer a 50Hz
    // TODO: Impostare duty cycle iniziale per home position

    servo_state.pan_deg = 0.0;
    servo_state.tilt_deg = 0.0;
}

/**
 * TODO: Convertire gradi in larghezza impulso PWM
 * @param deg: angolo in gradi
 * @return: larghezza impulso in microsecondi (1000-2000)
 */
static uint16_t degrees_to_us(float deg) {
    // Formula: us = 1000 + (deg / 180.0) * 1000
    // Assumendo 0° = 1000us, 180° = 2000us
    // float us = SERVO_PULSE_MIN_US + (deg / 180.0f) * SERVO_PULSE_RANGE_US;
    // return (uint16_t)us;
    return 1500; // placeholder
}

/**
 * TODO: Impostare angolo pan
 * @param deg: angolo in gradi (verrà clampato ai limiti)
 */
void servo_pan_set(float deg) {
    // Clamp to limits
    // deg = (deg > servo_state.pan_max) ? servo_state.pan_max : deg;
    // deg = (deg < servo_state.pan_min) ? servo_state.pan_min : deg;

    // Apply offset
    // deg += SERVO_PAN_OFFSET_DEG;

    // Convert to PWM
    // uint16_t pulse_us = degrees_to_us(deg);

    // Set PWM duty cycle
    // __HAL_TIM_SET_COMPARE(&htim_servo, TIM_CHANNEL_PAN, pulse_us);

    servo_state.pan_deg = deg;
}

/**
 * TODO: Impostare angolo tilt
 * @param deg: angolo in gradi (verrà clampato ai limiti)
 */
void servo_tilt_set(float deg) {
    // Similar to pan
    // Clamp, offset, convert, set
    servo_state.tilt_deg = deg;
}

/**
 * TODO: Impostare entrambi gli angoli contemporaneamente
 * @param pan_deg: angolo pan in gradi
 * @param tilt_deg: angolo tilt in gradi
 */
void servo_set_position(float pan_deg, float tilt_deg) {
    servo_pan_set(pan_deg);
    servo_tilt_set(tilt_deg);
}

/**
 * TODO: Spostare i servo alla posizione home (0°, 0°)
 */
void servo_home(void) {
    servo_set_position(0.0, 0.0);
}

/**
 * TODO: Spostare i servo alla posizione safe (ruota rivolta in alto)
 * Tilt a 90° così la camera guarda dritto
 */
void servo_safe_position(void) {
    servo_set_position(0.0, 90.0);
}

/**
 * TODO: Ottenere posizioni attuali
 * @param pan: output per angolo pan (può essere NULL)
 * @param tilt: output per angolo tilt (può essere NULL)
 */
void servo_get_position(float *pan, float *tilt) {
    if (pan) *pan = servo_state.pan_deg;
    if (tilt) *tilt = servo_state.tilt_deg;
}

/**
 * TODO: Impostare limiti fisici dei servo
 * @param pan_min, pan_max: limiti pan in gradi
 * @param tilt_min, tilt_max: limiti tilt in gradi
 */
void servo_set_limits(float pan_min, float pan_max, float tilt_min, float tilt_max) {
    servo_state.pan_min = pan_min;
    servo_state.pan_max = pan_max;
    servo_state.tilt_min = tilt_min;
    servo_state.tilt_max = tilt_max;
}
