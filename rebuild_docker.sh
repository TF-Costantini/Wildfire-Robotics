#!/bin/bash
set -e

IMAGE_NAME=ros:humble
CONTAINER_NAME=ros2_dev
PROJ_FILES=./_LINUX
RTOS_FILES=./_RTOS

# Pulls ROS image
docker pull $IMAGE_NAME

# Removes the old container
docker container rm -f $CONTAINER_NAME

# Runs ros image detached.
#
# Mount layout:
#   /home/project/ros2_ws/src   -> ROS2 sources (MPU side)
#   /setup.sh                   -> bootstrap of the ROS2 stack
#   /home/project/test_images   -> sample images for vision tests
#   /home/project/_RTOS         -> firmware sources + setup_firmware.sh
#
# The _RTOS mount lets teammates build the MCU firmware (PlatformIO +
# micro-ROS) from inside the same container by running:
#   bash /home/project/_RTOS/setup_firmware.sh
docker run -itd \
  --name $CONTAINER_NAME \
  -v "$PROJ_FILES/src/ros2_ws/src:/home/project/ros2_ws/src" \
  -v "$PROJ_FILES/setup.sh:/setup.sh" \
  -v "$PROJ_FILES/start.sh:/start.sh" \
  -v "$PROJ_FILES/test_images:/home/project/test_images" \
  -v "$RTOS_FILES:/home/project/_RTOS" \
  $IMAGE_NAME

echo -e "\nSTARTED SUCCESSFULLY\n"

# Attaches to the container
docker exec -it $CONTAINER_NAME /bin/bash