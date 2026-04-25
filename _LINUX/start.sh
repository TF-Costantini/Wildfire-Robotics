#!/bin/bash
set -e

#goes to ros2_ws
cd src/ros2_ws

#updates linux
apt update

#updates the local cache of rosdep's dependency database
rosdep update

#installs libraries and dependencies to build the proj
rosdep install --from-paths src --ignore-src -y

#to build ros2_ws
colcon build