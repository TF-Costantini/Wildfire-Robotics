#!/bin/bash
set -e

CONTAINER_NAME=ros2_dev

#Attaches to the container if it exists
docker exec -it $CONTAINER_NAME /bin/bash