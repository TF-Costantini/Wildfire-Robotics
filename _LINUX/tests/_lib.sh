#!/bin/bash
# Shared helpers for all test scripts.
# Sourced from each test_NN_*.sh — defines container name, env sourcing,
# and shorthand functions for invoking commands inside the running ROS2 container.

# Name of the running ROS2 development container (started by rebuild_docker.sh).
CONTAINER=ros2_dev

# Single string that sources both the ROS2 distro and the workspace overlay.
# Reused by every test so we don't repeat it.
SETUP='source /opt/ros/humble/setup.bash && source /home/project/ros2_ws/install/setup.bash'

# Run a one-shot command inside the container with ROS env sourced.
# Usage: dexec '<cmd>'
dexec() {
    docker exec "$CONTAINER" bash -lc "$SETUP && $1"
}

# Run a command inside the container as a background/detached process.
# Used to launch a ROS2 node and immediately move on to publish/echo helpers.
# Usage: dexec_bg '<cmd>'
dexec_bg() {
    docker exec -d "$CONTAINER" bash -lc "$SETUP && $1"
}

# Kill any process inside the container whose name matches the regex.
# Used at the end of each test to tear down nodes started in background.
# Usage: dkill <pattern>
dkill() {
    docker exec "$CONTAINER" pkill -f "$1" >/dev/null 2>&1 || true
}

# Pretty headers / footers for terminal output.
hdr() { printf '\n========== %s ==========\n' "$1"; }
ok()  { printf '  [OK] %s\n' "$1"; }
ko()  { printf '  [KO] %s\n' "$1"; }

# Verify the container is up before any test runs.
require_container() {
    if ! docker ps --format '{{.Names}}' | grep -qx "$CONTAINER"; then
        echo "ERROR: container '$CONTAINER' is not running. Start it with ./rebuild_docker.sh or ./start_docker.sh"
        exit 1
    fi
}