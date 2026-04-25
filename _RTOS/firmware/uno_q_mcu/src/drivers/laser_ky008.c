/**
 * laser_ky008.c — Wildfire Robotics UGV
 * TODO: Implementare driver per laser KY-008
 *
 * Il KY-008 è un puntatore laser rosso 5V:
 * - Signal pin: digitale on/off
 * - 5V VCC
 * - GND
 *
 * Attenzione: non guardare mai direttamente il laser!
 */

#include "laser_ky008.h"
#include "pins.h"

static bool laser_state = false;

/**
 * TODO: Inizializzare il laser
 * Configurare GPIO come output, laser spento
 */
void laser_init(void) {
    // TODO: Abilitare clock GPIO
    // TODO: Configurare PIN_LASER come output
    // TODO: Spegnere laser inizialmente
    laser_state = false;
    // HAL_GPIO_WritePin(LASER_PORT, PIN_LASER, GPIO_PIN_RESET);
}

/**
 * TODO: Accendere il laser
 */
void laser_on(void) {
    laser_state = true;
    // HAL_GPIO_WritePin(LASER_PORT, PIN_LASER, GPIO_PIN_SET);
}

/**
 * TODO: Spegnere il laser
 */
void laser_off(void) {
    laser_state = false;
    // HAL_GPIO_WritePin(LASER_PORT, PIN_LASER, GPIO_PIN_RESET);
}

/**
 * TODO: Toggle laser stato
 */
void laser_toggle(void) {
    if (laser_state) {
        laser_off();
    } else {
        laser_on();
    }
}

/**
 * TODO: Ottenere stato corrente
 * @return: true se acceso
 */
bool laser_is_on(void) {
    return laser_state;
}
