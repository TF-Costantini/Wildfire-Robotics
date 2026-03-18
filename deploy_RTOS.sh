#!/bin/bash
## RUN THIS FROM Configuration Run and specify ENV there to maximize flexibility

arduino-cli compile --fqbn arduino:zephyr:unoq "$SRC_DIR" --output-dir "$BIN_DIR"

ssh -i "$PEM_PATH" "$REMOTE" "mkdir -p $REMOTE_DIR"
scp -i "$PEM_PATH" -r "$BIN_DIR"/* "$REMOTE":"$REMOTE_DIR"

arduino-cli upload --fqbn arduino:zephyr:unoq --input-file "$REMOTE_DIR"/main.ino.elf

ssh -i "$PEM_PATH" "$REMOTE" "arduino-cli upload --fqbn arduino:zephyr:unoq --input-file $REMOTE_DIR/main.ino.elf"