# CMakeLists.txt per firmware micro-ROS
# Wildfire Robotics UGV
# TODO: Implementare se si usa CMake invece di PlatformIO

cmake_minimum_required(VERSION 3.16)
project(uno_q_mcu)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# TODO: Trovare toolchain ARM GCC
# include(cmake/toolchain/arm-none-eabi-gcc.cmake)

# TODO: Definire MCU
# set(TARGET_MCU "STM32U585AI")

# Source files
# set(SOURCES
#     src/main.c
#     src/drivers/motor_tb6612.c
#     src/drivers/servo_mg996r.c
#     src/drivers/ultrasonic_hcsr04.c
#     src/drivers/laser_ky008.c
#     src/drivers/button.c
# )

# Include directories
# include_directories(
#     include
#     src/drivers
# )

# TODO: micro-ROS dependencies
# find_package(microros_msgs REQUIRED)

# TODO: Build executable
# add_executable(${PROJECT_NAME}.elf ${SOURCES})

# TODO: Linker flags per STM32U585
# target_link_libraries(${PROJECT_NAME}.elf ...)

# TODO: Convert to binary for flashing
# add_custom_command(OUTPUT ${PROJECT_NAME}.bin
#     COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
#     DEPENDS ${PROJECT_NAME}.elf
# )
