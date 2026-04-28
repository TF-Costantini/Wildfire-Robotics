#!/bin/bash
set -e

MAIN_FOLDER="./firmware/uno_q_mcu/main"
BIN_OUT="./firmware/bin"

clear

arduino-cli \
  compile "$MAIN_FOLDER" \
    --fqbn arduino:zephyr:unoq \
    --output-dir "$BIN_OUT" \
    --library "$MAIN_FOLDER/src/drivers" \
    --library "$MAIN_FOLDER/include"