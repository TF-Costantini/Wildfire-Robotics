/**
 * Servo.h — host stub of the Arduino Servo library used by
 * servo_mg996r.cpp under unit tests.
 *
 * Records the last attached pin and the last microseconds value so
 * tests can assert the degrees→μs mapping.
 */

#ifndef SERVO_H_HOST_SHIM
#define SERVO_H_HOST_SHIM

class Servo {
public:
    int last_pin       = -1;
    int last_min_us    = -1;
    int last_max_us    = -1;
    int last_us        = -1;
    int last_deg       = -1;
    int attach_count   = 0;

    void attach(int pin, int min_us = 1000, int max_us = 2000) {
        last_pin    = pin;
        last_min_us = min_us;
        last_max_us = max_us;
        attach_count += 1;
    }

    void detach(void) { last_pin = -1; }

    void writeMicroseconds(int us) { last_us = us; }
    void write(int deg)            { last_deg = deg; }

    int readMicroseconds(void) const { return last_us; }
    int read(void)              const { return last_deg; }
};

#endif
