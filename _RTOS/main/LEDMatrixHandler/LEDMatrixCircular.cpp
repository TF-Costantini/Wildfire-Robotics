//
// Created by Tommaso F. Costantini on 18/03/26.
//

#include <cstring>
#include <LEDMatrixCircular.h>

void LEDMatrixCircular::goForward(ArduinoLEDMatrix* matrix) {
    uint8_t full[LED_MAT_H][LED_MAT_W] = {};

    for (int i = 0; i < LED_MAT_W; i++) {
        fillColumn(reinterpret_cast<uint8_t*>(full), i, 1);
        matrix->renderBitmap(full, LED_MAT_H, LED_MAT_W);
        delay(100);
        matrix->clear();
        memset(full, 0, sizeof(full));
    }
}

void LEDMatrixCircular::goBackwards(ArduinoLEDMatrix* matrix) {}
