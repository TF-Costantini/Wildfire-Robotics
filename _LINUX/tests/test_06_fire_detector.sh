#!/bin/bash
# Verify fire_detector_node detects red/orange blobs in a static test image.
#
# Pipeline:
#   camera_node (test_image_path mode) -> /camera/image_raw  -> fire_detector_node -> /vision/fire
#
# We rely on the test_images/ folder mounted at /home/project/test_images
# inside the container (see rebuild_docker.sh).
#
# Expected: with test_fire.jpg containing fire-colour pixels, /vision/fire
# should publish found=true and a non-zero area.

set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/_lib.sh"
require_container

IMG=/home/project/test_images/test_fire.jpg

# Sanity-check the image is reachable from inside the container.
if ! docker exec "$CONTAINER" test -f "$IMG"; then
    ko "test image $IMG not found inside container"; exit 1
fi
ok "test image present: $IMG"

hdr "Start camera_node in test-image mode"
# When test_image_path is set, camera_node skips VideoCapture and republishes
# the same JPG at 15 fps — perfect for offline detector tests.
dexec_bg "ros2 run wildfire_vision camera_node --ros-args -p test_image_path:=$IMG"
sleep 2

hdr "Start fire_detector_node"
dexec_bg "ros2 run wildfire_vision fire_detector_node"
sleep 2

hdr "Sample /vision/fire"
# Read 3 messages — the first one may arrive before the detector finished
# initialising; we want to see a stable found=true response.
dexec "timeout 4 ros2 topic echo /vision/fire | head -n 30"

hdr "Cleanup"
dkill camera_node
dkill fire_detector_node
ok "done"