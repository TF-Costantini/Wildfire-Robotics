# STM32 Flash Pipeline

When I refer to:
- *Linux* I mean on the Linux - Qualcomm side of the board.
- *RTOS* I mean the RTOS - STM32 side of the board.

## Method 1: Standard Arduino
This will accept .ino files mixed with .cpp, but you have to follow the Arduino standard for it to compile.

You can:
- Copy the source code into Linux and compile inside (SLOWER)
- Compile on your machine, then copy the *.elf-zsk.bin* into Linux and flash it.

### Option 1: Compile Locally (Faster)

#### On your machine:
- **Compile:**
`arduino-cli compile --fqbn arduino:zephyr:unoq <SRC_DIR>`

- **Copy:**
`scp -i <YOUR_PEM_PATH> -r <BIN_DIR> arduino@phoenix.local:~/zephyr_bin`

#### SSH into Arduino Linux
- **Flash:**
`arduino-cli upload --fqbn arduino:zephyr:unoq ~/zephyr_bin`


### Option 2: Compile on Arduino (Slower)

#### On your machine:
- **Copy:**
  `scp -i <YOUR_PEM_PATH> -r <SRC_DIR> arduino@phoenix.local:~/zephyr_src`

#### SSH into Arduino Linux
- **Compile:**
  `arduino-cli compile --fqbn arduino:zephyr:unoq ~/zephyr_src`

- **Flash:**
  `arduino-cli upload --fqbn arduino:zephyr:unoq ~/zephyr_src`


## Method 2: Frank Sinatra Way
WIP