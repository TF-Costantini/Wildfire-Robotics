#!/bin/bash
set -e

BIN_DIR="./firmware/bin/"
REMOTE_DIR=/home/arduino/zephyr_bin
DEVICE_SERIAL="3038275042"

./compile.sh

adb -s "$DEVICE_SERIAL" shell "mkdir -p $REMOTE_DIR"
adb -s "$DEVICE_SERIAL" push "$BIN_DIR" "$REMOTE_DIR"
adb -s "$DEVICE_SERIAL" shell "arduino-cli upload -v --fqbn arduino:zephyr:unoq --input-file $REMOTE_DIR/bin/main.ino.elf"

echo -e "\nRESTARTING CONTAINER...\n"
adb -s "$DEVICE_SERIAL" shell "docker restart ros2_dev"