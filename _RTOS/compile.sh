#!/bin/bash
set -e

BIN_DIR="./firmware/bin"
MAIN_FOLDER="./firmware/uno_q_mcu/main"

clear

arduino-cli \
  compile "$MAIN_FOLDER" \
    --fqbn arduino:zephyr:unoq \
    --output-dir "$BIN_DIR" \
    --library "$MAIN_FOLDER/src/drivers" \
    --library "$MAIN_FOLDER/include"