#!/bin/bash
set -e

BASE_PATH="/_LINUX"
ARDUINO_BASE="/home/arduino"
PEM_PATH=/Volumes/POLI/.ssh/auq_phoenix.pem
REMOTE=arduino@phoenix.local

echo -e "\nCOPYING ALL REQUIRED DATA...\n"

scp -i "$PEM_PATH" -r ".$BASE_PATH/src/." "$REMOTE:$ARDUINO_BASE$BASE_PATH/src/"
scp -i "$PEM_PATH" -r ".$BASE_PATH/test_images/." "$REMOTE:$ARDUINO_BASE$BASE_PATH/test_images"
scp -i "$PEM_PATH" -r ".$BASE_PATH/setup.sh" "$REMOTE:$ARDUINO_BASE$BASE_PATH/setup.sh"
scp -i "$PEM_PATH" -r ".$BASE_PATH/start.sh" "$REMOTE:$ARDUINO_BASE$BASE_PATH/start.sh"
scp -i "$PEM_PATH" -r "./rebuild_docker.sh" "$REMOTE:$ARDUINO_BASE/rebuild_docker.sh"
scp -i "$PEM_PATH" -r "./start_docker.sh" "$REMOTE:$ARDUINO_BASE/start_docker.sh"

echo -e "\nRESTARTING CONTAINER...\n"
ssh -i "$PEM_PATH" "$REMOTE" "docker restart ros2_dev"

