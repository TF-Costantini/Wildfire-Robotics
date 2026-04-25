#!/bin/bash
set -e

IMAGE_NAME=ros:humble
CONTAINER_NAME=ros2_dev
PROJ_FILES=./_LINUX/src

#Pulls ROS image
docker pull $IMAGE_NAME

#Removes container
docker container rm -f $CONTAINER_NAME

#Runs ros image
docker run -it --name $CONTAINER_NAME -v "$PROJ_FILES:/home/project" $IMAGE_NAME

echo -e "\nSTARTED SUCCESSFULLY"