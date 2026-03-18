# STM32 Flash Pipeline

When I refer to:
- *Linux* I mean on the Linux - Qualcomm side of the board.
- *RTOS* I mean the RTOS - STM32 side of the board.

## Standard Arduino
This will accept .ino files mixed with .cpp, but you have to follow the Arduino standard for it to compile.

You can:
- Copy the source code into Linux and compile inside (SLOWER)
- Compile on your machine, then copy the *.elf-zsk.bin* into Linux and flash it.

**STEPS:**

1) **Compile:**
`arduino-cli compile --fqbn arduino:zephyr:unoq ~/SRC_DIR`

2) **Copy:**
`scp`

3) **Flash:**
`arduino-cli upload --fqbn arduino:zephyr:unoq ~/SRC_DIR`

