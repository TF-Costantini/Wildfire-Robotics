# tests/Makefile — host build for the firmware driver unit tests.
#
# Compiles the driver .cpp files against the Arduino API stub in this
# directory. Run from the firmware project root or directly here:
#
#     make -C _RTOS/firmware/uno_q_mcu/tests
#     make -C _RTOS/firmware/uno_q_mcu/tests run
#     make -C _RTOS/firmware/uno_q_mcu/tests clean

CXX      ?= c++
CXXFLAGS ?= -std=c++17 -O0 -g -Wall -Wextra -Wno-unused-parameter

TESTS_DIR    := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
PROJECT_DIR  := $(abspath $(TESTS_DIR)/..)
INCLUDE_DIR  := $(PROJECT_DIR)/include
DRIVERS_DIR  := $(PROJECT_DIR)/src/drivers

# tests/ is FIRST so our Arduino.h / Servo.h shims override the
# real headers when the host toolchain happens to ship them.
INCLUDES := -I$(TESTS_DIR) -I$(INCLUDE_DIR) -I$(PROJECT_DIR)/src

DRIVER_SRCS := \
    $(DRIVERS_DIR)/motor_bts7960.cpp     \
    $(DRIVERS_DIR)/servo_mg996r.cpp      \
    $(DRIVERS_DIR)/button.cpp            \
    $(DRIVERS_DIR)/laser_ky008.cpp       \
    $(DRIVERS_DIR)/ultrasonic_hcsr04.cpp

STUB_SRCS := \
    $(TESTS_DIR)/arduino_stub.cpp

TEST_SRCS := $(TESTS_DIR)/test_drivers.cpp

BIN := $(TESTS_DIR)/test_drivers

.PHONY: all run clean

all: $(BIN)

$(BIN): $(DRIVER_SRCS) $(STUB_SRCS) $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

run: $(BIN)
	$(BIN)

clean:
	rm -f $(BIN)
