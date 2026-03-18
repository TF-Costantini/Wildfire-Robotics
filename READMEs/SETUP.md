# Project Setup and IDE Config

The setup is intended for CLion/Rider or any JetBrains suite products.
It will likely be similar with any other IDE.

### IMPORTANT:
Initial board setup required Arduino App Lab.

**DO NOT CHANGE BOARD NAME** `phoenix` as mDNS will fail. We are using `phoenix.local` as the IP address.

Use the app only to specify which WI-FI network it should connect to.

**NOTE**:
It's not that you can't change the board name. But if you do, SSH host must be changed for all users.
A different board will have a different name for DNS resolution in the same LAN and its own connection parameters

### IMPORTANT VALUES:
- **BOARD_IP:** `phoenix.local`

## Install Arduino-Cli & Zephyr profile (optional)

*MANDATORY IF YOU WANT TO COMPILE LOCALLY*

### Install arduino-cli
**Mac:** `brew install arduino-cli`

**Windows/Linux:** *figure it out*


### Install zephyr-profile
**All:** `arduino-cli core install arduino:zephyr`


### Check for Success
**All:** `arduino-cli core list`

**Expected Output:**
```
ID                  Installed       Latest Name
arduino:zephyr      0.53.1          0.53.1 Arduino UNO Q Board
```

### Install all dependencies
`arduino-cli lib install "MsgPack"`


## Create SSH Connection profile to Arduino

- **Host:** `<BOARD_IP>`
- **Username:** `arduino`
- **Private key file:** *ASK ENGINEERING TEAM*

<img src="READMEs/IMGs/SETUP/SSH_Setup.png" width="80%" alt="SSH Connection Profile Image">

## Create BUILD & FLASH Configuration
### IMPORTANT:
The script compiles locally and copies binaries. Therefore, the *optional* step is **MANDATORY** for this to work.

Running the script will:
1) Compile Locally
2) Copy the files on the Arduino Linux OS
3) Flash the binary on the RTOS. It will automatically start.

### Run Configuration
- **Script Path:** `<BASE_REPO>/deploy_RTOS.sh`
- **Working Directory:** `<BASE_REPO>`

<img src="READMEs/IMGs/SETUP/BuildFlashConfig.png" width="80%" alt="SSH Connection Profile Image">

### Run Configuration ENV
- **SRC_DIR:** `./_RTOS/main`
- **BIN_DIR:** `./_RTOS/bin`
- **PEM_PATH:** `<YOUR_PEM_PATH>`
- **REMOTE:** `arduino@<BOARD_IP>`
- **REMOTE_DIR:** `/home/arduino/zephyr_bin`

<img src="READMEs/IMGs/SETUP/BuildFlashENV.png" width="80%" alt="SSH Connection Profile Image">
