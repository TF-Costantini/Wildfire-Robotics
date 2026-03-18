//
// Created by Tommaso F. Costantini on 18/03/26.
//

#include <LEDMatrixBounce.h>

void LEDMatrixBounce::goForward(ArduinoLEDMatrix* matrix) {
    byte full[LED_MAT_H][LED_MAT_W];
    memset(full, 0, sizeof(full));

    for (int i = 0; i < LED_MAT_W; i++) {
        fillColumn((byte*) full, i, 1);
        matrix->renderBitmap(full, LED_MAT_H, LED_MAT_W);
        delay(100);
        matrix->clear();
        memset(full, 0, sizeof(full));
    }
}

void LEDMatrixBounce::goBackwards(ArduinoLEDMatrix* matrix) {
    byte full[LED_MAT_H][LED_MAT_W];
    memset(full, 0, sizeof(full));

    for (int i = LED_MAT_W - 1; i >= 0; i--) {
        fillColumn((byte*) full, i, 1);
        matrix->renderBitmap(full, LED_MAT_H, LED_MAT_W);
        delay(100);
        matrix->clear();
        memset(full, 0, sizeof(full));
    }
}