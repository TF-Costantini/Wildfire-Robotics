#!/bin/bash
set -e

nohup ros2 launch wildfire_bringup bringup.launch.py use_sim:=false > ~/all_nodes.log 2>&1 &