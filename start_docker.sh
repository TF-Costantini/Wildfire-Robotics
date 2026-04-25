#!/bin/bash
set -e

CONTAINER_NAME=ros2_dev

#Starts and attaches to the container if it exists
docker start -ai $CONTAINER_NAME