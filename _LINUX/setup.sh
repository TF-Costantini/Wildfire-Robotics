#!/bin/bash
set -e

SRC_PATH="/home/project/ros2_ws"
MICROROS_WS="/home/project/microros_ws"
ROSDEP_FILE="/etc/ros/rosdep/sources.list.d/50-wildfire.list"
ROSDEP_ENTRY="yaml file://$SRC_PATH/src/wildfire_vision/rosdep/python3-ultralytics.yaml"

cd $SRC_PATH

# Installing python-3 and core build tools.
# Note: python3-venv, build-essential and binutils are required by the
# MCU firmware build flow (see _RTOS/setup_firmware.sh). They are
# installed here so a single `bash setup.sh` bootstraps the container
# for both the ROS2 stack and the firmware build.
echo -e "\INSTALLING PYTHON AND EXTERNAL DEPENDENCIES\n"
apt-get install -y \
    python3-pip python3-venv \
    build-essential binutils \
    git curl ca-certificates libusb-1.0-0
pip3 install ultralytics

# Updates all
echo -e "\nUPDATING LINUX\n"
apt update && apt upgrade

# Adds Ultralytics to RosDep ONLY if missing
if ! grep -qF "$ROSDEP_ENTRY" "$ROSDEP_FILE" 2>/dev/null; then
  echo "$ROSDEP_ENTRY" >> "$ROSDEP_FILE"
fi

# Updates the local cache of rosdep dependency database
echo -e "\nUPDATING ROS DEPENDENCY DATABASE\n"
rosdep update

# Installs libraries and dependencies to build the proj
echo -e "\nINSTALLING ROS REQUIRED DEPENDENCY\n"
rosdep install --from-paths ./src --ignore-src -y --rosdistro humble

# Removing old build files
rm -rf "$SRC_PATH/build"
rm -rf "$SRC_PATH/install"
rm -rf "$SRC_PATH/log"

# To build ros2_ws
echo -e "\nBUILDING ROS2_WS\n"
source /opt/ros/humble/setup.bash
touch "$SRC_PATH/build.log"
colcon build --event-handlers console_direct+ > "$SRC_PATH/build.log" --cmake-clean-cache

# Sources the setup file
echo -e "\nSOURCING INSTALL FILE\n"
source "$SRC_PATH/install/setup.bash"

# Check Success
echo -e "CHECKING PACKAGES EXIST...\n"
ros2 pkg list | grep wildfire || { echo -e "PACKAGES NOT FOUND. BUILD FAILED."; false; }
echo -e "\nPACKAGES FOUND! BUILD SUCCESSFUL!\n"

# =====================================================================
# micro-ROS Agent (host side)
# ---------------------------------------------------------------------
# The Agent is the bridge between the MCU's micro-ROS client (running on
# the STM32U585 inside the UNO Q) and the ROS2 graph that lives on the
# MPU/host. Without it, the topics published by the firmware never
# appear on /ultrasonic/*, /button_event, /cmd_*_mcu, etc.
#
# Build flow (official micro-ROS Foundation procedure for Humble):
#   1. clone github.com/micro-ROS/micro_ros_setup into a separate ws
#   2. colcon build the setup package
#   3. ros2 run micro_ros_setup create_agent_ws.sh   (downloads sources)
#   4. ros2 run micro_ros_setup build_agent.sh       (compiles agent)
#
# The first build downloads ~150 MB and takes ~1-3 minutes on the UNO Q
# Dragonwing MPU. It is idempotent: re-running the script skips work
# that is already done, so it is safe to put here.
# =====================================================================
echo -e "\nINSTALLING micro-ROS AGENT (host bridge)\n"
mkdir -p "$MICROROS_WS/src"
cd "$MICROROS_WS"

if [ ! -d "src/micro_ros_setup" ]; then
    git clone -b humble https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup
fi

source /opt/ros/humble/setup.bash
rosdep install --from-paths src --ignore-src -y

# Build the setup package itself (provides create_agent_ws.sh / build_agent.sh).
colcon build --packages-select micro_ros_setup
source "$MICROROS_WS/install/local_setup.bash"

# Pull agent sources only on first run (script is idempotent).
if [ ! -d "src/uros/micro-ROS-Agent" ]; then
    ros2 run micro_ros_setup create_agent_ws.sh
fi

# Compile the agent itself only if not already built.
if [ ! -x "$MICROROS_WS/install/micro_ros_agent/lib/micro_ros_agent/micro_ros_agent" ]; then
    ros2 run micro_ros_setup build_agent.sh
fi

# Verify
if [ ! -x "$MICROROS_WS/install/micro_ros_agent/lib/micro_ros_agent/micro_ros_agent" ]; then
    echo "micro_ros_agent BUILD FAILED" >&2
    false
fi
echo -e "\nmicro-ROS Agent built: $MICROROS_WS/install/micro_ros_agent\n"

cd "$SRC_PATH"

# Adding sources to ./bashrc if not existing yet so that they are sourced automatically on new sessions
grep -qxF 'source /home/project/ros2_ws/install/setup.bash' ~/.bashrc || echo 'source /home/project/ros2_ws/install/setup.bash' >> ~/.bashrc
grep -qxF 'source /opt/ros/humble/setup.bash' ~/.bashrc || echo 'source /opt/ros/humble/setup.bash' >> ~/.bashrc
grep -qxF 'source /home/project/microros_ws/install/local_setup.bash' ~/.bashrc || echo 'source /home/project/microros_ws/install/local_setup.bash' >> ~/.bashrc
grep -qxF 'export PATH="$HOME/.platformio/penv/bin:$PATH"' ~/.bashrc || echo 'export PATH="$HOME/.platformio/penv/bin:$PATH"' >> ~/.bashrc

# To also build the MCU firmware (PlatformIO + micro-ROS), run:
#   bash /home/project/_RTOS/setup_firmware.sh
# It is intentionally NOT run here because the first build downloads
# ~1 GB of toolchains + micro-ROS sources (~10 min) and not every
# teammate needs the firmware built every time.
echo -e "\nMCU firmware build is optional. Run when needed:"
echo -e "    bash /home/project/_RTOS/setup_firmware.sh\n"