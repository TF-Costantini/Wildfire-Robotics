/**
 * button.h — Wildfire Robotics UGV
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

// Tipi di evento pulsante
#define BUTTON_EVENT_NONE    0
#define BUTTON_EVENT_PRESS   1
#define BUTTON_EVENT_RELEASE 2

// === API PUBBLICA ===

/**
 * Inizializzare il pulsante
 * Configurare GPIO come input con pull-up e debounce
 */
void button_init(void);

/**
 * Processare il pulsante (da chiamare nel main loop o timer)
 * Gestisce debounce software e genera eventi
 * @return: tipo evento (BUTTON_EVENT_NONE, BUTTON_EVENT_PRESS, BUTTON_EVENT_RELEASE)
 */
uint8_t button_process(void);

/**
 * Verificare se il pulsante è premuto (raw)
 * @return: true se premuto
 */
bool button_is_pressed(void);

/**
 * Ottenere ultimo timestamp press
 * @return: timestamp in ms
 */
uint32_t button_last_press_time(void);

#endif // BUTTON_H
