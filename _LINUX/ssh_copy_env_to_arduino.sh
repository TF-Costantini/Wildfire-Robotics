#!/bin/bash
set -e

BASE_PATH="/_LINUX"
ARDUINO_BASE="/home/arduino"
PEM_PATH=/Volumes/POLI/.ssh/auq_phoenix.pem
REMOTE=arduino@phoenix.local

echo -e "\nSYNCING...\n"

rsync -avz --delete -e "ssh -i $PEM_PATH" \
  "..$BASE_PATH/" "$REMOTE:$ARDUINO_BASE$BASE_PATH/"

rsync -avz -e "ssh -i $PEM_PATH" \
  "../rebuild_docker.sh" "../start_docker.sh" \
  "$REMOTE:$ARDUINO_BASE/"

echo -e "\nRESTARTING CONTAINER...\n"
ssh -i "$PEM_PATH" "$REMOTE" "docker restart ros2_dev"