#!/bin/bash
# setup_firmware.sh — Wildfire Robotics UGV
#
# Installs PlatformIO and all the prerequisites needed to build the
# MCU firmware (firmware/uno_q_mcu). Idempotent: safe to re-run.
#
# Works on:
#   - Linux  (Ubuntu/Debian, including the ros:humble docker container)
#   - macOS  (with Homebrew available)
#
# After install, the script also runs the first PlatformIO build,
# applying the known workaround for rmw_test_fixture if needed.
#
# Output: firmware/uno_q_mcu/.pio/build/uno_q_mcu/firmware.bin

set -e

# =====================================================================
# Paths
# =====================================================================
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
FIRMWARE_DIR="$SCRIPT_DIR/firmware/uno_q_mcu"
PENV_DIR="$HOME/.platformio/penv"
# Path of wildfire_msgs relative to firmware/uno_q_mcu/extra_packages/
WILDFIRE_MSGS_REL="../../../../_LINUX/src/ros2_ws/src/wildfire_msgs"

# Use sudo only if not already root and sudo is available.
if [ "$EUID" -ne 0 ] && command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
else
    SUDO=""
fi

echo "=== Wildfire MCU firmware setup ==="
echo "Firmware project: $FIRMWARE_DIR"

# =====================================================================
# 1. System dependencies
# =====================================================================
case "$(uname)" in
    Linux)
        echo "[1/4] Installing system deps (apt)..."
        $SUDO apt-get update -qq
        $SUDO apt-get install -y --no-install-recommends \
            python3 python3-venv python3-pip \
            git curl ca-certificates \
            build-essential binutils \
            libusb-1.0-0
        ;;
    Darwin)
        echo "[1/4] Installing system deps (brew)..."
        if ! command -v brew >/dev/null 2>&1; then
            echo "Homebrew not found. Install it first: https://brew.sh"
            exit 1
        fi
        if [ ! -d /opt/homebrew/opt/binutils ] && \
           [ ! -d /usr/local/opt/binutils ]; then
            brew install binutils
        fi
        ;;
    *)
        echo "Unsupported OS: $(uname)"
        exit 1
        ;;
esac

# =====================================================================
# 2. PlatformIO into the venv path required by micro_ros_platformio
# =====================================================================
echo "[2/4] Installing PlatformIO into $PENV_DIR ..."
if [ ! -x "$PENV_DIR/bin/pio" ]; then
    python3 -m venv "$PENV_DIR"
    "$PENV_DIR/bin/pip" install --upgrade --quiet pip
    "$PENV_DIR/bin/pip" install --quiet platformio
fi

# Make pio resolvable in the rest of the script.
export PATH="$PENV_DIR/bin:/opt/homebrew/opt/binutils/bin:/usr/local/opt/binutils/bin:$PATH"
echo "    PlatformIO: $(pio --version)"

# =====================================================================
# 3. Symlink wildfire_msgs into extra_packages so micro_ros_platformio
#    picks it up during the typesupport build.
# =====================================================================
echo "[3/4] Linking wildfire_msgs into firmware/extra_packages/ ..."
mkdir -p "$FIRMWARE_DIR/extra_packages"
ln -sfn "$WILDFIRE_MSGS_REL" "$FIRMWARE_DIR/extra_packages/wildfire_msgs"

# =====================================================================
# 4. Build firmware (with rmw_test_fixture workaround)
# =====================================================================
echo "[4/4] Building firmware ..."
cd "$FIRMWARE_DIR"

build_with_retry() {
    if pio run -e uno_q_mcu; then
        return 0
    fi
    local ignore_path=".pio/libdeps/uno_q_mcu/micro_ros_platformio/build/dev/src/ament_cmake_ros/rmw_test_fixture"
    if [ -d "$ignore_path" ] && [ ! -f "$ignore_path/COLCON_IGNORE" ]; then
        echo ""
        echo "Applying COLCON_IGNORE workaround on rmw_test_fixture ..."
        touch "$ignore_path/COLCON_IGNORE"
        pio run -e uno_q_mcu
    else
        return 1
    fi
}

if ! build_with_retry; then
    echo ""
    echo "Firmware build failed. See output above for details."
    exit 1
fi

echo ""
echo "=== Firmware build OK ==="
echo "Output: $FIRMWARE_DIR/.pio/build/uno_q_mcu/firmware.bin"
echo ""
echo "Flash with: pio run -e uno_q_mcu -t upload   (board attached via ST-LINK)"
echo "Monitor:    pio device monitor -b 115200"
