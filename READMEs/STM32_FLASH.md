# STM32 Flash Pipeline

When I refer to:
- *Linux* I mean on the Linux - Qualcomm side of the board.
- *RTOS* I mean the RTOS - STM32 side of the board.

## Method 1: Standard Arduino
This will accept `.ino` files mixed with `.cpp`, but you have to follow the Arduino standard for it to compile.

You can:
- Compile on your machine, then copy the `.elf-zsk.bin` or the whole binaries directory into Linux and flash it. (FASTER)
- Copy the source code into Linux and compile inside and flash (SLOWER)

### Option 1: Compile Locally (Faster)

#### On your machine:
- **Compile:**
`arduino-cli compile --fqbn arduino:zephyr:unoq <SRC_DIR> --output-dir <BIN_DIR>`

- **Copy:**
`scp -i <YOUR_PEM_PATH> -r <BIN_DIR> arduino@phoenix.local:~/zephyr_bin`

#### SSH into Arduino Linux
- **Flash:**
`arduino-cli upload --fqbn arduino:zephyr:unoq --input-file ~/zephyr_bin/main.ino.elf`


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
This method completely disregards Arduino and works with the 'raw' STM32U585.
Write `.cpp` and compile with `west`

**PROs:**
- More control over the hardware
- Access to all the primitives
- No setup() loop()
- No arduino-limited folder structure
- Signing binaries not necessary

**CONs:**
- Lose access to Monitor
- Lose access to Bridge, RouterBridge (no gRPC but really not needed)

### Useful links & docs
- [Project Zephyr - Arduino Uno Q Documentation](https://docs.zephyrproject.org/latest/boards/arduino/uno_q/doc/index.html)

### How and Where to Flash + Recovery
- **RECOVER ARDUINO BOOTLOADER (ICE):** `arduino-cli burn-bootloader -b arduino:zephyr:unoq -P jlink`


- **Tool:** `remoteocd` - Already installed on the board
- **@ Address:** `0x08000000` - Overwriting Arduino Bootloader
- **Flash Config:** `custom_flash.cfg` - [Custom Flash Config File](../_LINUX/FLASH/custom_flash.cfg)
- **CMD:** `remoteocd upload -f <CFG_PATH> <BIN_PATH>`

### How to Compile and Flash
WIP