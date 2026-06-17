#!/bin/bash
set -e

BIN_DIR="./firmware/bin"
PEM_PATH=~/.ssh/auq_phoenix.pem
REMOTE=arduino@phoenix.local
REMOTE_DIR=/home/arduino/zephyr_bin

./compile.sh

ssh -i "$PEM_PATH" "$REMOTE" "mkdir -p $REMOTE_DIR"
scp -i "$PEM_PATH" -r "$BIN_DIR"/* "$REMOTE:$REMOTE_DIR"

ssh -i "$PEM_PATH" "$REMOTE" "arduino-cli upload --fqbn arduino:zephyr:unoq --input-file $REMOTE_DIR/main.ino.elf"