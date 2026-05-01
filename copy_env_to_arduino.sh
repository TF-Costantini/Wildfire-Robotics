#!/bin/bash
set -e

BASE_PATH="/_LINUX"
ARDUINO_BASE="/home/arduino"

echo -e "\nCOPYING ALL REQUIRED DATA...\n"

adb push ".$BASE_PATH/src/." "$ARDUINO_BASE$BASE_PATH/src/"
adb push ".$BASE_PATH/test_images/." "$ARDUINO_BASE$BASE_PATH/test_images"
adb push ".$BASE_PATH/setup.sh" "$ARDUINO_BASE$BASE_PATH/setup.sh"
adb push ".$BASE_PATH/start.sh" "$ARDUINO_BASE$BASE_PATH/start.sh"
adb push "./rebuild_docker.sh" "$ARDUINO_BASE/rebuild_docker.sh"
adb push "./start_docker.sh" "$ARDUINO_BASE/start_docker.sh"

echo -e "\nRESTARTING CONTAINER...\n"
adb shell "docker restart ros2_dev"

