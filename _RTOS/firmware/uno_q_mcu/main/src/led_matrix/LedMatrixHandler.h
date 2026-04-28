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

    static void fillColumn(uint8_t* ledMat, int col, uint8_t val)
    {
        for (int row = 0; row < LED_MAT_H; row++)
            setLed(ledMat, row, col, val);
    }

    static void setLed(uint8_t* ledMat, int row, int col, uint8_t val)
    {
        ledMat[row * LED_MAT_W + col] = val;
    }
};

#endif //REPO_LEDMATRIXHANDLER_H
