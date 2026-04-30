#!/bin/bash
set -e

# CAMERA NODE
nohup ros2 run wildfire_vision camera_node --ros-args -p test_image_path:="/home/project/test_images/test_fire.jpg" > ~/camera_node.log 2>&1 &

# PERSON DETECTOR NODE
nohup ros2 run wildfire_vision person_detector_node > ~/person_detector.log 2>&1 &

# FIRE DETECTOR NODE
nohup ros2 run wildfire_vision fire_detector_node > ~/fire_detector.log 2>&1 &

# STATE MACHINE NODE
nohup ros2 run wildfire_control state_machine_node > ~/state_machine.log 2>&1 &

# FOLLOW CONTROLLER NODE
nohup ros2 run wildfire_control follow_controller_node > ~/follow_controller.log 2>&1 &

# FIRE CONTROLLER NODE
nohup ros2 run wildfire_control fire_controller_node > ~/fire_controller_node.log 2>&1 &