#!/bin/bash
set -e

SRC_PATH="/home/project/ros2_ws"
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

cd "$SRC_PATH"

# Adding sources to ./bashrc if not existing yet so that they are sourced automatically on new sessions
grep -qxF 'source /home/project/ros2_ws/install/setup.bash' ~/.bashrc || echo 'source /home/project/ros2_ws/install/setup.bash' >> ~/.bashrc
grep -qxF 'source /opt/ros/humble/setup.bash' ~/.bashrc || echo 'source /opt/ros/humble/setup.bash' >> ~/.bashrc
grep -qxF 'source /home/project/microros_ws/install/local_setup.bash' ~/.bashrc || echo 'source /home/project/microros_ws/install/local_setup.bash' >> ~/.bashrc
grep -qxF 'export PATH="$HOME/.platformio/penv/bin:$PATH"' ~/.bashrc || echo 'export PATH="$HOME/.platformio/penv/bin:$PATH"' >> ~/.bashrc