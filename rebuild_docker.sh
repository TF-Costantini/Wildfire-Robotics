#!/bin/bash
set -e

IMAGE_NAME=ros:humble
CONTAINER_NAME=ros2_dev
PROJ_FILES=./_LINUX

# Pulls ROS image
docker pull $IMAGE_NAME

# Removes the old container
docker container rm -f $CONTAINER_NAME

# Runs ros image detached
docker run -itd \
  --name $CONTAINER_NAME \
  -v "$PROJ_FILES/src/ros2_ws/src:/home/project/ros2_ws/src" \
  -v "$PROJ_FILES/setup.sh:/setup.sh" \
  -v "$PROJ_FILES/test_images:/home/project" \
  $IMAGE_NAME

echo -e "\nSTARTED SUCCESSFULLY\n"

# Attaches to the container
docker exec -it $CONTAINER_NAME /bin/bash