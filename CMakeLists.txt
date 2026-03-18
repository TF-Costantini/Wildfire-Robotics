cmake_minimum_required(VERSION 4.1)
project(Repo)

set(CMAKE_CXX_STANDARD 14)

include_directories(
        ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.53.1/cores/arduino
        ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.53.1/libraries/Arduino_LED_Matrix/src
        ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.53.1/libraries/Arduino_RouterBridge/src

        _RTOS/main
        _RTOS/main/LEDMatrixHandler
)

add_executable(Repo
        _RTOS/main/LEDMatrixHandler/LEDMatrixBounce.cpp
        _RTOS/main/LEDMatrixHandler/LEDMatrixBounce.h
        _RTOS/main/LEDMatrixHandler/LEDMatrixCircular.cpp
        _RTOS/main/LEDMatrixHandler/LEDMatrixCircular.h
        _RTOS/main/LEDMatrixHandler/LEDMatrixHandler.h
        _RTOS/main/main.ino)
