//
// Created by Tommaso F. Costantini on 28/04/2026.
//


#ifndef REPO_LEDMATRIXHANDLER_H
#define REPO_LEDMATRIXHANDLER_H

#include <Arduino_LED_Matrix.h>
#include <atomic>

#define LED_MAT_H 8  //correct value = 8
#define LED_MAT_W 13  //correct value = 13

#define STD_ON_FOR_N_LOOPS 20 // Number of loops for which the led should stay on
#define BUTTON_ON_FOR_N_LOOPS 200 // Number of loops for which the led should stay on

inline Arduino_LED_Matrix matrix;
inline uint8_t ledBitmap[LED_MAT_H][LED_MAT_W] = {};


inline std::atomic_int left_motor_on_count = 0;
inline std::atomic_int right_motor_on_count = 0;

inline std::atomic_int camera_on_count = 0;

inline std::atomic_int left_ultrasonic_on_count = 0;
inline std::atomic_int right_ultrasonic_on_count = 0;

inline std::atomic_int button_on_count = 0;

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

    static void left_motor_on()
    {
        left_motor_on_count.store(STD_ON_FOR_N_LOOPS);
    }

    static void right_motor_on()
    {
        right_motor_on_count.store(STD_ON_FOR_N_LOOPS);
    }

    static void left_ultrasonic_on()
    {
        left_ultrasonic_on_count.store(STD_ON_FOR_N_LOOPS);
    }

    static void right_ultrasonic_on()
    {
        right_ultrasonic_on_count.store(STD_ON_FOR_N_LOOPS);
    }

    static void camera_on()
    {
        camera_on_count.store(STD_ON_FOR_N_LOOPS);
    }

    static void button_on()
    {
        button_on_count.store(BUTTON_ON_FOR_N_LOOPS);
    }

    static void handle_loop_decrease()
    {
        left_motor_decrease();
        right_motor_decrease();
        camera_decrease();
        left_ultrasonic_decrease();
        right_ultrasonic_decrease();
        button_decrease();
    }

    static void handle_loop_apply()
    {
        update_led_left_ultrasonic();
        update_led_right_ultrasonic();
        update_led_camera();
        update_led_left_motor();
        update_led_right_motor();
        update_led_button();
    }


private:

    static void left_motor_decrease()
    {
        if (left_motor_on_count.load() == 0) return;
        --left_motor_on_count;
    }

    static void update_led_left_motor()
    {
        const bool value = left_motor_on_count.load() > 0;

        setValueInBitMap(0, 7, value);
        setValueInBitMap(0, 8, value);
        setValueInBitMap(0, 9, value);
        setValueInBitMap(1, 7, value);
        setValueInBitMap(1, 8, value);
        setValueInBitMap(1, 9, value);
    }

    static void right_motor_decrease()
    {
        if (right_motor_on_count.load() == 0) return;
        --right_motor_on_count;
    }

    static void update_led_right_motor()
    {
        const bool value = right_motor_on_count.load() > 0;

        setValueInBitMap(5, 7, value);
        setValueInBitMap(5, 8, value);
        setValueInBitMap(5, 9, value);
        setValueInBitMap(6, 7, value);
        setValueInBitMap(6, 8, value);
        setValueInBitMap(6, 9, value);
    }

    static void left_ultrasonic_decrease()
    {
        if (left_ultrasonic_on_count.load() == 0) return;
        --left_ultrasonic_on_count;
    }

    static void update_led_left_ultrasonic()
    {
        const bool value = left_ultrasonic_on_count.load() > 0;

        setValueInBitMap(0, 12, value);
        setValueInBitMap(1, 12, value);
    }

    static void right_ultrasonic_decrease()
    {
        if (right_ultrasonic_on_count.load() == 0) return;
        --right_ultrasonic_on_count;
    }

    static void update_led_right_ultrasonic()
    {
        const bool value = right_ultrasonic_on_count.load() > 0;

        setValueInBitMap(5, 12, value);
        setValueInBitMap(6, 12, value);
    }

    static void camera_decrease()
    {
        if (camera_on_count.load() == 0) return;
        --camera_on_count;
    }

    static void update_led_camera()
    {
        const bool value = camera_on_count.load() > 0;

        setValueInBitMap(3, 10, value);
        setValueInBitMap(3, 11, value);
        setValueInBitMap(4, 10, value);
        setValueInBitMap(4, 11, value);
    }

    static void button_decrease()
    {
        if (button_on_count.load() == 0) return;
        --button_on_count;
    }

    static void update_led_button()
    {
        const bool value = button_on_count.load() > 0;

        setValueInBitMap(3, 7, value);
        setValueInBitMap(3, 6, value);
        setValueInBitMap(4, 7, value);
        setValueInBitMap(4, 6, value);
    }

};

#endif //REPO_LEDMATRIXHANDLER_H
