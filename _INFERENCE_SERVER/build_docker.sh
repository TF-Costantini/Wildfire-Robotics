#!/bin/bash
set -e

docker build --platform linux/arm64 -t inference-server ./src