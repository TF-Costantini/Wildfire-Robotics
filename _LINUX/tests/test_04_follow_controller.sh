#!/bin/bash
# Exercise follow_controller_node with synthetic inputs.
#
# Inputs needed by the controller:
#   /mode               (Mode)             — must be FOLLOW(1) for it to act
#   /vision/person      (Detection)        — fake person centroid
#   /ultrasonic/left    (sensor_msgs/Range) — distance in metres
#   /ultrasonic/right   (sensor_msgs/Range)
#
# Expected output on /cmd_drive given parameters in params.yaml:
#   target_distance=80cm, min_safe=40cm, linear_kp=0.01, angular_kp=0.5
#
# Scenarios covered:
#   A. person centred, distance 1.5m  -> drive forward, both wheels positive, equal
#   B. person off to the right        -> turn right (left>right)
#   C. distance 0.30m  (< safe)       -> stop (left=right=0)
#   D. found=false                    -> stop

set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/_lib.sh"
require_container

# Launch follow controller in background.
hdr "Start follow_controller_node"
dexec_bg "ros2 run wildfire_control follow_controller_node"
sleep 2

# Inject /mode = FOLLOW(1) continuously so the node leaves the gating branch.
hdr "Set /mode = FOLLOW"
dexec_bg "ros2 topic pub -r 5 /mode wildfire_msgs/msg/Mode \
'{mode: 1, stamp: {sec: 0, nanosec: 0}}'"
sleep 1

# Helper: publish ultrasonic distance (in metres) on left+right at 10 Hz.
# Range fields min_range/max_range are required by follow_controller validation.
publish_ultrasonics() {
    local d="$1"
    dkill 'topic pub.*ultrasonic'  # remove previous distance publishers
    dexec_bg "ros2 topic pub -r 10 /ultrasonic/left sensor_msgs/msg/Range \
'{radiation_type: 0, field_of_view: 0.5, min_range: 0.02, max_range: 4.0, range: $d}'"
    dexec_bg "ros2 topic pub -r 10 /ultrasonic/right sensor_msgs/msg/Range \
'{radiation_type: 0, field_of_view: 0.5, min_range: 0.02, max_range: 4.0, range: $d}'"
}

# Helper: publish a person Detection at 10 Hz.
publish_person() {
    local found="$1" cx="$2" cy="$3"
    dkill 'topic pub.*vision/person'
    dexec_bg "ros2 topic pub -r 10 /vision/person wildfire_msgs/msg/Detection \
'{found: $found, cx: $cx, cy: $cy, area: 10000.0, img_w: 640.0, img_h: 480.0, confidence: 0.9, use_confidence: true}'"
}

sample_drive() { dexec "timeout 2 ros2 topic echo /cmd_drive --once 2>/dev/null"; }

# ── Scenario A: person centred at (320,240), distance 1.5m → forward ──────────
hdr "A) person centred, distance=1.5m -> expect both wheels positive, near equal"
publish_ultrasonics 1.5
publish_person true 320.0 240.0
sleep 2
sample_drive

# ── Scenario B: person at right edge, same distance → turn right (left>right) ─
hdr "B) person at cx=600 (right), distance=1.5m -> expect left>right"
publish_person true 600.0 240.0
sleep 2
sample_drive

# ── Scenario C: distance below safety threshold → stop ───────────────────────
hdr "C) distance=0.30m (< 0.40m safe) -> expect left=right=0 (safety stop)"
publish_ultrasonics 0.30
publish_person true 320.0 240.0
sleep 2
sample_drive

# ── Scenario D: lost detection → stop ───────────────────────────────────────
hdr "D) found=false -> expect left=right=0"
publish_person false 0.0 0.0
sleep 2
sample_drive

hdr "Cleanup"
dkill 'topic pub'
dkill follow_controller_node
ok "done"