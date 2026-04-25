#!/bin/bash
set -e

SRC_PATH=/home/project/ros2_ws

cd $SRC_PATH

# Updates Linux runtime
echo -e "\nUPDATING LINUX\n"
apt update

# Updates the local cache of rosdep dependency database
echo -e "\nUPDATING ROS DEPENDENCY DATABASE\n"
rosdep update

# Installs libraries and dependencies to build the proj
echo -e "\nINSTALLING ROS REQUIRED DEPENDENCY\n"
rosdep install --from-paths . --ignore-src -y --rosdistro humble

# Removing old build files
rm -rf "$SRC_PATH/build"
rm -rf "$SRC_PATH/install"
rm -rf "$SRC_PATH/log"

# To build ros2_ws
echo -e "\nBUILDING ROS2_WS\n"
source /opt/ros/humble/setup.bash
colcon build --event-handlers console_direct+ > build.log

# Sources the setup file
echo -e "\nSOURCING INSTALL FILE\n"
source "$SRC_PATH/install/setup.bash"

# Check Success
echo -e "CHECKING PACKAGES EXIST...\n"
ros2 pkg list | grep wildfire || { echo -e "PACKAGES NOT FOUND. BUILD FAILED."; false; }
echo -e "\nPACKAGES FOUND! BUILD SUCCESSFUL!\n"
