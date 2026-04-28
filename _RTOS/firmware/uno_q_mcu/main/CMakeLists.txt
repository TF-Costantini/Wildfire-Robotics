cmake_minimum_required(VERSION 4.1)
project(Repo)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.53.1/cores/arduino
        ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.53.1/libraries/Arduino_LED_Matrix/src
        ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.53.1/libraries/Arduino_RouterBridge/src
        ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.53.1/libraries/Servo/src

        include
)


add_executable(Repo
        main.ino
        src/drivers/motor_bts7960.cpp
        src/drivers/servo_mg996r.cpp
        src/drivers/ultrasonic_hcsr04.cpp
        src/drivers/laser_ky008.cpp
        src/drivers/button.cpp

        src/drivers/motor_bts7960.h
        src/drivers/servo_mg996r.h
        src/drivers/ultrasonic_hcsr04.h
        src/drivers/laser_ky008.h
        src/drivers/button.h
        src/led_matrix/LedMatrixHandler.h
)