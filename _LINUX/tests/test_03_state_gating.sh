#!/bin/bash
# Verify mutual exclusion / gating logic of state_machine_node.
#
# Behaviour under test (from IMPLEMENTATION.md):
#   IDLE   -> /cmd_drive_mcu == 0/0, /cmd_pantilt_mcu == home, /cmd_laser_mcu == false
#   FOLLOW -> /cmd_drive_mcu forwards /cmd_drive, pantilt -> home, laser -> false
#   FIRE   -> /cmd_drive_mcu forced to 0/0, pantilt + laser forwarded
#
# Strategy: in each mode, inject a "dirty" command on /cmd_drive and /cmd_pantilt
# and /cmd_laser, then read the corresponding /cmd_*_mcu topic and check the
# values match the mode's contract.

set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/_lib.sh"
require_container

# Start state machine and a continuous publisher of dirty commands so the
# state machine has something to forward / gate.
hdr "Start state_machine_node"
dexec_bg "ros2 run wildfire_control state_machine_node"
sleep 2

# Continuously publish DriveCmd(left=0.7,right=0.7) at 10 Hz so the state
# machine always has a recent dirty value when we sample /cmd_drive_mcu.
hdr "Inject dirty /cmd_drive (0.7, 0.7) at 10 Hz"
dexec_bg "ros2 topic pub -r 10 /cmd_drive wildfire_msgs/msg/DriveCmd \
'{left: 0.7, right: 0.7, stamp: {sec: 0, nanosec: 0}}'"

# Same for /cmd_pantilt (45,15) and /cmd_laser (true).
dexec_bg "ros2 topic pub -r 10 /cmd_pantilt wildfire_msgs/msg/PanTiltCmd \
'{pan_deg: 45.0, tilt_deg: 15.0, stamp: {sec: 0, nanosec: 0}}'"
dexec_bg "ros2 topic pub -r 10 /cmd_laser std_msgs/msg/Bool '{data: true}'"
sleep 2

# Helper: send one button press to advance the state machine.
press_button() {
    dexec "ros2 topic pub --once /button_event wildfire_msgs/msg/ButtonEvent \
'{stamp: {sec: 0, nanosec: 0}, kind: 0}' >/dev/null"
    sleep 1
}

# Helper: snapshot one sample of a topic.
sample() { dexec "timeout 2 ros2 topic echo $1 --once 2>/dev/null"; }

hdr "STATE = IDLE — drive must be 0/0, laser false"
echo "drive_mcu:";   sample /cmd_drive_mcu
echo "pantilt_mcu:"; sample /cmd_pantilt_mcu
echo "laser_mcu:";   sample /cmd_laser_mcu

press_button
hdr "STATE = FOLLOW — drive must forward 0.7/0.7, pantilt forced to home, laser false"
echo "drive_mcu:";   sample /cmd_drive_mcu
echo "pantilt_mcu:"; sample /cmd_pantilt_mcu
echo "laser_mcu:";   sample /cmd_laser_mcu

press_button
hdr "STATE = FIRE — drive forced 0/0, pantilt forwards (45,15), laser forwards true"
echo "drive_mcu:";   sample /cmd_drive_mcu
echo "pantilt_mcu:"; sample /cmd_pantilt_mcu
echo "laser_mcu:";   sample /cmd_laser_mcu

hdr "Cleanup"
# Kill the dirty publishers and the node.
dkill 'topic pub'
dkill state_machine_node
ok "done"