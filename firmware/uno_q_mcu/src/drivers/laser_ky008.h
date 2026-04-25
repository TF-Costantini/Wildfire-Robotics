/**
 * laser_ky008.h — Wildfire Robotics UGV
 */

#ifndef LASER_KY008_H
#define LASER_KY008_H

#include <stdbool.h>

// === API PUBBLICA ===

/**
 * Inizializzare il laser KY-008
 * Configurare GPIO come output
 */
void laser_init(void);

/**
 * Accendere il laser
 */
void laser_on(void);

/**
 * Spegnere il laser
 */
void laser_off(void);

/**
 * Toggle laser stato
 */
void laser_toggle(void);

/**
 * Ottenere stato corrente
 * @return: true se acceso
 */
bool laser_is_on(void);

#endif // LASER_KY008_H
