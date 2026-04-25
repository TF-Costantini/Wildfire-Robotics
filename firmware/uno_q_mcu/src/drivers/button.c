/**
 * button.c — Wildfire Robotics UGV
 * TODO: Implementare driver per pulsante con debounce
 *
 * Il pulsante è collegato tra GPIO e GND con pull-up interno.
 * Il debounce HW (RC network) + SW assicura letture stabili.
 */

#include "button.h"
#include "pins.h"

// === COSTANTI ===
#define DEBOUNCE_TIME_MS     50      // Tempo di debounce in ms

// === STATI INTERNI ===
static struct {
    bool raw_state;          // Stato raw (non debounced)
    bool stable_state;       // Stato stabile dopo debounce
    uint32_t last_change_ms; // Ultimo cambio di stato (ms)
    uint32_t press_time_ms;  // Timestamp ultima press
} button_state = {false, false, 0, 0};

/**
 * TODO: Inizializzare il pulsante
 */
void button_init(void) {
    // TODO: Abilitare clock GPIO
    // TODO: Configurare PIN_BUTTON come input con pull-up
    // TODO: Opzionale: configurare_EXTI per interrupt on change
    button_state.raw_state = false;
    button_state.stable_state = false;
    button_state.last_change_ms = 0;
    button_state.press_time_ms = 0;
}

/**
 * TODO: Leggere stato raw del pulsante
 * @return: true se premuto (GPIO low, pull-up)
 */
static bool button_read_raw(void) {
    // return !HAL_GPIO_ReadPin(BUTTON_PORT, PIN_BUTTON);  // Invertito perché pull-up
    return false;  // placeholder
}

/**
 * TODO: Processare pulsante con debounce software
 * @return: evento (BUTTON_EVENT_NONE, BUTTON_EVENT_PRESS, BUTTON_EVENT_RELEASE)
 */
uint8_t button_process(void) {
    uint32_t now_ms = 0;  // TODO: usare HAL_GetTick() o simile
    // now_ms = HAL_GetTick();

    // Leggere stato raw
    bool raw = button_read_raw();

    // Se stato raw è cambiato
    if (raw != button_state.raw_state) {
        button_state.raw_state = raw;
        button_state.last_change_ms = now_ms;
    }

    // Se è passato tempo sufficiente dall'ultimo cambio
    if ((now_ms - button_state.last_change_ms) >= DEBOUNCE_TIME_MS) {
        // Se stato stabile è diverso da stato attuale
        if (button_state.stable_state != button_state.raw_state) {
            button_state.stable_state = button_state.raw_state;

            // Generare evento
            if (button_state.stable_state) {
                // Press
                button_state.press_time_ms = now_ms;
                return BUTTON_EVENT_PRESS;
            } else {
                // Release
                return BUTTON_EVENT_RELEASE;
            }
        }
    }

    return BUTTON_EVENT_NONE;
}

/**
 * TODO: Verificare se premuto (raw)
 * @return: true se premuto
 */
bool button_is_pressed(void) {
    return button_read_raw();
}

/**
 * TODO: Ottenere timestamp ultima press
 * @return: timestamp in ms
 */
uint32_t button_last_press_time(void) {
    return button_state.press_time_ms;
}
