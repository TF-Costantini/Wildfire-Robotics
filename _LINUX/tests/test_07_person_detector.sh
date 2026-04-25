#!/bin/bash
# Verify person_detector_node detects a person in a static test image.
#
# Pipeline:
#   camera_node (test_image_path mode) -> /camera/image_raw -> person_detector_node -> /vision/person
#
# The first run downloads yolov8n.pt (~6 MB) into the working dir of the node
# and loads PyTorch — that can take 10–30s. Subsequent runs are fast.
#
# Expected with test_person.jpg: /vision/person publishes found=true, a
# centroid near the centre of the image, and a confidence > 0.5.

set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/_lib.sh"
require_container

IMG=/home/project/test_images/test_person.jpg

# Sanity-check the image is reachable from inside the container.
if ! docker exec "$CONTAINER" test -f "$IMG"; then
    ko "test image $IMG not found inside container"; exit 1
fi

# Make sure ultralytics is installed inside the container, otherwise the node
# will load with self._model=None and only emit found=false. We don't fail
# the test here — just warn.
if ! docker exec "$CONTAINER" python3 -c 'import ultralytics' >/dev/null 2>&1; then
    echo "WARN: ultralytics not installed in container — install with:"
    echo "  docker exec $CONTAINER pip install ultralytics"
fi

hdr "Start camera_node in test-image mode"
dexec_bg "ros2 run wildfire_vision camera_node --ros-args -p test_image_path:=$IMG"
sleep 2

hdr "Start person_detector_node (this may take ~30s on the first run)"
dexec_bg "ros2 run wildfire_vision person_detector_node"
# Generous wait: model load + first inference.
sleep 25

hdr "Sample /vision/person"
dexec "timeout 6 ros2 topic echo /vision/person | head -n 30"

hdr "Cleanup"
dkill camera_node
dkill person_detector_node
ok "done"