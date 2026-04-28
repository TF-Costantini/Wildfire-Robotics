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
        LEDMatrixHandler/LEDMatrixBounce.cpp
        LEDMatrixHandler/LEDMatrixBounce.h
        LEDMatrixHandler/LEDMatrixCircular.cpp
        LEDMatrixHandler/LEDMatrixCircular.h
        LEDMatrixHandler/LEDMatrixHandler.h
        main.ino)
