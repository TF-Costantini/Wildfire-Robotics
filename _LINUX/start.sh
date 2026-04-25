#!/bin/bash
set -e

SRC_PATH=/home/project/src/ros2_ws

cd $SRC_PATH

# Updates Linux runtime
echo -e "\nUPDATING LINUX\n"
apt update

# Updates the local cache of rosdep dependency database
echo -e "\nUPDATING ROS DEPENDENCY DATABASE\n"
rosdep update

# Installs libraries and dependencies to build the proj
echo -e "\nINSTALLING ROS REQUIRED DEPENDENCY\n"
rosdep install --from-paths . --ignore-src -y

# To build ros2_ws
echo -e "\nBUILDING ROS2_WS\n"
colcon build