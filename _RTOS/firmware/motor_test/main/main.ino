#include <Arduino.h>
#include "motor_bts7960.h"

int pos = 0;    // variable to store the servo position

void setup() {
    motor_init();

    motor_set(1.0f, 1.0f);
}

void loop() {
}