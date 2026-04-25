#!/bin/bash
# Smoke test: launch every node one at a time and verify it survives 3 seconds
# without crashing. Catches obvious import errors, parameter typos, missing
# dependencies. No publish/echo logic — just "does it boot?".

set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/_lib.sh"
require_container

# List of (package, executable) tuples to smoke-test.
NODES=(
    "wildfire_control state_machine_node"
    "wildfire_control follow_controller_node"
    "wildfire_control fire_controller_node"
    "wildfire_vision fire_detector_node"
    # NOTE: person_detector_node not included by default — first run downloads
    # yolov8n.pt (~6 MB) and starts torch, which is slow. See test_07.
)

for entry in "${NODES[@]}"; do
    pkg="${entry%% *}"
    exe="${entry##* }"
    hdr "Smoke $pkg / $exe"

    # Start the node in the background inside the container.
    dexec_bg "ros2 run $pkg $exe"

    # Wait long enough for ROS init + first timer tick.
    sleep 3

    # Check process is still alive — if it crashed during init, pgrep returns non-zero.
    if docker exec "$CONTAINER" pgrep -f "$exe" >/dev/null; then
        ok "$exe is alive after 3s"
    else
        ko "$exe died during startup"
    fi

    # Tear it down before launching the next one.
    dkill "$exe"
    sleep 1
done

hdr "Smoke complete"