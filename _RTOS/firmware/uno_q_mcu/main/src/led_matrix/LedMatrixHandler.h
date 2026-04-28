//
// Created by Tommaso F. Costantini on 28/04/2026.
//


#ifndef REPO_LEDMATRIXHANDLER_H
#define REPO_LEDMATRIXHANDLER_H

#include <Arduino_LED_Matrix.h>
#include <cstdint>

#define LED_MAT_H 8  //correct value = 8
#define LED_MAT_W 13  //correct value = 13

inline Arduino_LED_Matrix matrix;
inline uint8_t ledBitmap[LED_MAT_H][LED_MAT_W] = {};

class LEDMatrixHandler
{
public:

    static void applyToLed(ArduinoLEDMatrix* matrix)
    {
        matrix->renderBitmap(ledBitmap, LED_MAT_H, LED_MAT_W);
    }

    static void setValueInBitMap(const int row, const int col, const bool state)
    {
        ledBitmap[row][col] = state;
    }

    static void setColInBitmap(const int col, const bool state)
    {
        for (int row = 0; row < LED_MAT_H; row++)
            setValueInBitMap(row, col, state);
    }


};

#endif //REPO_LEDMATRIXHANDLER_H
