#!/bin/bash
set -e

BASE_PATH="/_LINUX"
ARDUINO_BASE="/home/arduino"

echo -e "\nCOPYING ALL REQUIRED DATA...\n"

adb sync ".$BASE_PATH/src" "$ARDUINO_BASE$BASE_PATH/src"
adb sync ".$BASE_PATH/test_images" "$ARDUINO_BASE$BASE_PATH/test_images"
adb sync ".$BASE_PATH/setup.sh" "$ARDUINO_BASE$BASE_PATH/setup.sh"
adb sync ".$BASE_PATH/start.sh" "$ARDUINO_BASE$BASE_PATH/start.sh"
adb sync "./rebuild_docker.sh" "$ARDUINO_BASE/rebuild_docker.sh"
adb sync "./start_docker.sh" "$ARDUINO_BASE/start_docker.sh"

echo -e "\nRESTARTING CONTAINER...\n"
adb shell "docker restart ros2_dev"

