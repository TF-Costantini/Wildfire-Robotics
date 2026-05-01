#!/bin/bash
set -e

BASE_PATH='/home/project'
SRC_PATH="$BASE_PATH/ros2_ws"

cd $SRC_PATH

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

# Adding sources to ./bashrc if not existing yet so that they are sourced automatically on new sessions
grep -qxF 'source /home/project/ros2_ws/install/setup.bash' ~/.bashrc || echo 'source /home/project/ros2_ws/install/setup.bash' >> ~/.bashrc
grep -qxF 'source /opt/ros/humble/setup.bash' ~/.bashrc || echo 'source /opt/ros/humble/setup.bash' >> ~/.bashrc

# STARTING NODES
echo -e "\nSTARTING NODES\n"
nohup ros2 launch wildfire_bringup bringup.launch.py use_sim:=false > ~/all_nodes.log 2>&1 &

echo -e "\nSTARTING SEQUENCE COMPLETE\n"