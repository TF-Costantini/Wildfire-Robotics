#!/bin/bash
# Verify the IDLE -> FOLLOW -> FIRE -> IDLE rotation in state_machine_node.
# Strategy:
#   1. start state_machine_node
#   2. read /mode (latched) to confirm initial state == IDLE (0)
#   3. publish a ButtonEvent PRESS, read /mode again -> expect FOLLOW (1)
#   4. publish another PRESS -> expect FIRE (2)
#   5. publish another PRESS -> expect IDLE (0) again
# Each publish is a one-shot ros2 topic pub call, the echo --once blocks
# until a single message arrives or the timeout expires.

set -u
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
source "$SCRIPT_DIR/_lib.sh"
require_container

hdr "Start state_machine_node"
# Launch the state machine in background so subsequent commands can interact with it.
dexec_bg "ros2 run wildfire_control state_machine_node"
sleep 2

# Helper: print the current /mode by grabbing one latched message.
# `--once` exits as soon as one sample is received; QoS Reliable+TransientLocal
# means we always get the latest value even if we subscribe after publication.
read_mode() {
    dexec "timeout 3 ros2 topic echo /mode --once 2>/dev/null | grep '^mode:' | awk '{print \$2}'"
}

# Helper: send one button PRESS event. ButtonEvent.PRESS == 0.
press_button() {
    dexec "ros2 topic pub --once /button_event wildfire_msgs/msg/ButtonEvent \
'{stamp: {sec: 0, nanosec: 0}, kind: 0}' >/dev/null"
    # Allow callback + republish to propagate before reading /mode.
    sleep 1
}

hdr "Initial state should be IDLE (0)"
M=$(read_mode); [[ "$M" == "0" ]] && ok "mode=$M (IDLE)" || ko "expected 0, got '$M'"

hdr "Press 1 -> expect FOLLOW (1)"
press_button
M=$(read_mode); [[ "$M" == "1" ]] && ok "mode=$M (FOLLOW)" || ko "expected 1, got '$M'"

hdr "Press 2 -> expect FIRE (2)"
press_button
M=$(read_mode); [[ "$M" == "2" ]] && ok "mode=$M (FIRE)" || ko "expected 2, got '$M'"

hdr "Press 3 -> expect IDLE (0)"
press_button
M=$(read_mode); [[ "$M" == "0" ]] && ok "mode=$M (IDLE)" || ko "expected 0, got '$M'"

hdr "Cleanup"
dkill state_machine_node
ok "done"