/**
 * servo_mg996r.h — Wildfire Robotics UGV
 */

#ifndef SERVO_MG996R_H
#define SERVO_MG996R_H

#include <stdint.h>
#include <stdbool.h>

// === API PUBBLICA ===

/**
 * Inizializzare i servo MG996R
 */
void servo_init(void);

/**
 * Impostare angolo pan
 * @param deg: angolo in gradi
 */
void servo_pan_set(float deg);

/**
 * Impostare angolo tilt
 * @param deg: angolo in gradi
 */
void servo_tilt_set(float deg);

/**
 * Impostare entrambi gli angoli
 * @param pan_deg: angolo pan in gradi
 * @param tilt_deg: angolo tilt in gradi
 */
void servo_set_position(float pan_deg, float tilt_deg);

/**
 * Spostare i servo alla posizione home (0°, 0°)
 */
void servo_home(void);

/**
 * Spostare i servo alla posizione safe
 */
void servo_safe_position(void);

/**
 * Ottenere posizioni attuali
 * @param pan: output per angolo pan (può essere NULL)
 * @param tilt: output per angolo tilt (può essere NULL)
 */
void servo_get_position(float *pan, float *tilt);

/**
 * Impostare limiti fisici dei servo
 */
void servo_set_limits(float pan_min, float pan_max, float tilt_min, float tilt_max);

#endif // SERVO_MG996R_H
