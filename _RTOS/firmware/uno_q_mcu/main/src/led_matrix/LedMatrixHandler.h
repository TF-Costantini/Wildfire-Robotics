    //
    // Created by Tommaso F. Costantini on 28/04/2026.
    //

    //
    // Created by Tommaso F. Costantini on 18/03/26.
    //

#ifndef REPO_LEDMATRIXHANDLER_H
#define REPO_LEDMATRIXHANDLER_H

#include <Arduino_LED_Matrix.h>

#define LED_MAT_H 8  //correct value = 8
#define LED_MAT_W 13  //correct value = 13


class LEDMatrixHandler
{
public:

    static void setLed(ArduinoLEDMatrix* matrix, const int row, const int col, const bool state)
    {
        uint8_t ledBitmap[LED_MAT_H][LED_MAT_W] = {};

        setValueInBitMap(*ledBitmap, row, col, state);

        matrix->renderBitmap(ledBitmap, LED_MAT_H, LED_MAT_W);
    }

    static void fillColumn(ArduinoLEDMatrix* matrix, const int col, const bool state)
    {
        uint8_t ledBitmap[LED_MAT_H][LED_MAT_W] = {};

        setColInBitmap(*ledBitmap, col, state);

        matrix->renderBitmap(ledBitmap, LED_MAT_H, LED_MAT_W);
    }

private:

    static void setValueInBitMap(uint8_t* ledBitmap, const int row, const int col, const bool state)
    {
        ledBitmap[row * LED_MAT_W + col] = state;
    }

    static void setColInBitmap(uint8_t* ledBitmap, const int col, const bool state)
    {
        for (int row = 0; row < LED_MAT_H; row++)
            setValueInBitMap(ledBitmap, row, col, state);
    }


};

#endif //REPO_LEDMATRIXHANDLER_H
