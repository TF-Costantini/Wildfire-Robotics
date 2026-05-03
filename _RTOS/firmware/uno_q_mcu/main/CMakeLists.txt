cmake_minimum_required(VERSION 3.20)
project(wildfire_mcu)

set(ZEPHYR_BASE ~/Library/Arduino15/packages/arduino/hardware/zephyr/0.54.1)
set(ZEPHYR_VARIANT ${ZEPHYR_BASE}/variants/arduino_uno_q_stm32u585xx)
set(ARDUINO_LIBS ~/Documents/Arduino/libraries)


include_directories(
        # Core
        ${ZEPHYR_BASE}/cores/arduino
        ${ZEPHYR_BASE}/cores/arduino/api/deprecated
        ${ZEPHYR_BASE}/cores/arduino/api/deprecated-avr-comp

        # Variant + Zephyr headers
        ${ZEPHYR_VARIANT}
        ${ZEPHYR_VARIANT}/llext-edk/include/zephyr/include
        ${ZEPHYR_VARIANT}/llext-edk/include/zephyr/include/zephyr
        ${ZEPHYR_VARIANT}/llext-edk/include/zephyr/include/generated

        # Bundled libraries
        ${ZEPHYR_BASE}/libraries/Arduino_LED_Matrix/src

        # User libraries
        ${ARDUINO_LIBS}/Arduino_RouterBridge/src
        ${ARDUINO_LIBS}/Arduino_RPClite/src
        ${ARDUINO_LIBS}/MsgPack
        ${ARDUINO_LIBS}/DebugLog
        ${ARDUINO_LIBS}/ArxTypeTraits
        ${ARDUINO_LIBS}/ArxContainer
        ${ARDUINO_LIBS}/Servo/src

        # Project headers
        include
)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "*.ino")
add_executable(
        wildfire_mcu ${SOURCES}
        src/drivers/motor_watchdog.h
)