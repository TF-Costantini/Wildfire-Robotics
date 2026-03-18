#!/bin/bash
### RUN THIS FROM Configuration Run and specify ENV there to maximize flexibility

# Stops immediately if any command fails
set -e

clear
arduino-cli \
  compile "$SRC_DIR" \
    --fqbn arduino:zephyr:unoq \
    --output-dir "$BIN_DIR" \
    --library "./_RTOS/main/LEDMatrixHandler"


ssh -i "$PEM_PATH" "$REMOTE" "mkdir -p $REMOTE_DIR"
scp -i "$PEM_PATH" -r "$BIN_DIR"/* "$REMOTE":"$REMOTE_DIR"

ssh -i "$PEM_PATH" "$REMOTE" "arduino-cli upload --fqbn arduino:zephyr:unoq --input-file $REMOTE_DIR/main.ino.elf"