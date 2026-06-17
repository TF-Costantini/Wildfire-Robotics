#!/bin/bash
set -e

IMAGE_NAME="inference-server"
CONTAINER_NAME="Wildfire-Inference-Server"

clear

docker stop $CONTAINER_NAME | true

docker rm -f $CONTAINER_NAME | true

docker run -itd \
  --name $CONTAINER_NAME \
  --restart unless-stopped \
  -p "8765:8765" \
  -p "8766:8766" \
  -p "8080:8080" \
  $IMAGE_NAME

docker logs $CONTAINER_NAME -f -n 20
