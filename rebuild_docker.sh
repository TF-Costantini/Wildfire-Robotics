#!/bin/bash
set -e

IMAGE_NAME=ros:humble
CONTAINER_NAME=ros2_dev
PROJ_FILES=./_LINUX

# Pulls ROS image
docker pull $IMAGE_NAME

# Removes the old container
docker container rm -f $CONTAINER_NAME

docker run -itd \
  --name $CONTAINER_NAME \
  -v "$PROJ_FILES/src:/home/project" \
  -v "$PROJ_FILES/setup.sh:/setup.sh" \
  -v "$PROJ_FILES/start.sh:/start.sh" \
  -v "$PROJ_FILES/test_images:/home/project/test_images" \
  -v /var/run/arduino-router.sock:/var/run/arduino-router.sock \
  --net=host \
  --restart unless-stopped \
  --device=/dev/video0 \
  $IMAGE_NAME

echo -e "\nSTARTED SUCCESSFULLY\n"

# Attaches to the container
docker exec -it $CONTAINER_NAME /bin/bash