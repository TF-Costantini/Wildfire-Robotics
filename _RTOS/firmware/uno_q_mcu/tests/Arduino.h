/**
 * Arduino.h shim for host unit tests.
 * Drivers include <Arduino.h>; the test Makefile adds tests/ to -I so
 * this file is picked up instead of the toolchain core.
 */

#ifndef ARDUINO_H_HOST_SHIM
#define ARDUINO_H_HOST_SHIM

#include "arduino_stub.h"

#endif