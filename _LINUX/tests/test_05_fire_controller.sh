#!/bin/bash
# Exercise fire_controller_node with synthetic /vision/fire input.
#
# Inputs:
#   /mode           = FIRE(2)         — required for the controller to leave gating
#   /vision/fire    = Detection       — drives the SWEEPING / TRACKING / LOCKED FSM
#
# Outputs to observe:
#   /cmd_pantilt    = PanTiltCmd      — must move during sweep, then settle on tracking
#   /cmd_laser      = std_msgs/Bool   — must turn ON only when LOCKED
#
# Scenarios:
#   A. mode != FIRE          -> pantilt held at (0,0), laser false
#   B. mode = FIRE, no fire  -> SWEEPING: pan oscillates between -60..+60
#   C. mode = FIRE, fire centred and stable -> TRACKING -> LOCKED -> laser true
#   D. fire disappears for >0.3s -> back to SWEEPING -> laser false

set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/_lib.sh"
require_container

# Helpers --------------------------------------------------------------------

set_mode() {
    dkill 'topic pub.*/mode '
    sleep 0.3
    dexec_bg "ros2 topic pub -r 5 /mode wildfire_msgs/msg/Mode \
'{mode: $1, stamp: {sec: 0, nanosec: 0}}'"
}

publish_fire() {
    local found="$1" cx="$2" cy="$3" area="$4"
    dkill 'topic pub.*vision/fire'
    dexec_bg "ros2 topic pub -r 10 /vision/fire wildfire_msgs/msg/Detection \
'{found: $found, cx: $cx, cy: $cy, area: $area, img_w: 640.0, img_h: 480.0, confidence: 0.0, use_confidence: false}'"
}

# Echo N pantilt samples to observe motion / convergence.
sample_pantilt_n() { dexec "timeout 4 ros2 topic echo /cmd_pantilt --no-arr 2>/dev/null | head -n $((6*$1))"; }
sample_laser()     { dexec "timeout 2 ros2 topic echo /cmd_laser --once 2>/dev/null"; }

# Test ----------------------------------------------------------------------

hdr "Start fire_controller_node"
dexec_bg "ros2 run wildfire_control fire_controller_node"
sleep 2

# ── A) mode=IDLE -> gating -> pantilt(0,0), laser false ─────────────────────
hdr "A) mode=IDLE -> pantilt held at (0,0), laser=false"
set_mode 0
sleep 1
sample_pantilt_n 1
sample_laser

# ── B) mode=FIRE, no detection -> SWEEPING (pan moves) ──────────────────────
hdr "B) mode=FIRE, found=false -> pan should oscillate (SWEEPING)"
set_mode 2
publish_fire false 0.0 0.0 0.0
sleep 4
sample_pantilt_n 4

# ── C) fire centred and stable for >lock_time -> LOCKED, laser true ─────────
# To force a lock with the default PD gains we pretend the fire is exactly at
# the image centre (320,240) so error_mag < lock_threshold (20px) immediately.
# After ~lock_time(0.5s) the FSM should report LOCKED and laser turns ON.
hdr "C) fire centred (320,240), area large -> expect LOCKED, laser=true"
publish_fire true 320.0 240.0 5000.0
sleep 3
sample_laser
sample_pantilt_n 1

# ── D) fire lost > unlock_time -> SWEEPING, laser false ─────────────────────
hdr "D) fire lost (found=false) for >0.3s -> SWEEPING, laser=false"
publish_fire false 0.0 0.0 0.0
sleep 2
sample_laser

hdr "Cleanup"
dkill 'topic pub'
dkill fire_controller_node
ok "done"