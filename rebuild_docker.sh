#!/bin/bash
set -e

IMAGE_NAME=ros:humble
CONTAINER_NAME=ros2_dev
PROJ_FILES=./_LINUX

#Pulls ROS image
docker pull $IMAGE_NAME

#Removes the old container
docker container rm -f $CONTAINER_NAME

#Runs ros image
docker run -it \
  --name $CONTAINER_NAME \
  -v "$PROJ_FILES/src/ros2_ws/src:/home/project/src/ros2_ws" \
  -v "$PROJ_FILES/start.sh:/start.sh" \
  $IMAGE_NAME

echo -e "\nSTARTED SUCCESSFULLY"