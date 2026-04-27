# micro-ROS Agent — Wildfire Robotics UGV

The micro-ROS Agent is the host-side bridge that lets the firmware
running on the STM32U585 inside the Arduino UNO Q publish/subscribe on
the ROS2 graph that lives on the MPU/Dragonwing side. Without it the
topics produced by the firmware never reach the rest of the stack:

- `/ultrasonic/left` / `/ultrasonic/right` (firmware → MPU)
- `/button_event` (firmware → MPU)
- `/cmd_drive_mcu` / `/cmd_pantilt_mcu` / `/cmd_laser_mcu` (MPU → firmware)

## What the Agent does in one sentence

It opens the serial port that exposes the MCU (`/dev/ttyACM0` via the
on-board ST-LINK on the UNO Q USB-C) and translates the micro-XRCE-DDS
session running on the firmware into a normal DDS participant that the
rest of the ROS2 graph can talk to.

## Install

The Agent is **not** distributed as an apt package for ROS 2 Humble. The
official build flow uses the helper repo `micro_ros_setup`. The
`_LINUX/setup.sh` script does this automatically — just run it once
from inside the docker container:

```
bash /setup.sh
```

What the script does (idempotent):

1. clones `micro_ros_setup` (Humble branch) into
   `/home/project/microros_ws/src/micro_ros_setup`
2. `colcon build --packages-select micro_ros_setup`
3. `ros2 run micro_ros_setup create_agent_ws.sh` — pulls the Agent
   sources (`micro-ROS-Agent`, `micro_ros_msgs`)
4. `ros2 run micro_ros_setup build_agent.sh` — compiles
   `micro_ros_agent` into
   `/home/project/microros_ws/install/micro_ros_agent`
5. appends `source /home/project/microros_ws/install/local_setup.bash`
   to `~/.bashrc`

After the script finishes you can verify:

```
which micro_ros_agent
# /home/project/microros_ws/install/micro_ros_agent/lib/micro_ros_agent/micro_ros_agent

ros2 run micro_ros_agent micro_ros_agent --help
```

## Run

The Agent is wired into `wildfire_bringup/launch/bringup.launch.py`
behind `UnlessCondition(use_sim)`. The default mode is hardware, so
`bringup.launch.py` will start the Agent automatically:

```
ros2 launch wildfire_bringup bringup.launch.py
```

This is equivalent to launching the rest of the stack plus, in the
background:

```
ros2 run micro_ros_agent micro_ros_agent serial \
    --dev /dev/ttyACM0 -b 115200
```

If your UNO Q enumerates on a different device path (e.g. when more
than one ST-LINK is connected), override the launch argument:

```
ros2 launch wildfire_bringup bringup.launch.py \
    agent_dev:=/dev/ttyACM1
```

You can also run the Agent standalone (useful for debugging without the
rest of the stack):

```
ros2 run micro_ros_agent micro_ros_agent serial \
    --dev /dev/ttyACM0 -b 115200 -v6
```

`-v6` enables full XRCE-DDS message tracing, very handy when the
firmware fails to negotiate the session.

## Simulated mode (no hardware)

When `use_sim:=true`, `bringup.launch.py` runs `mcu_sim_node` instead
and **does not start the Agent**. Use this on workstations without the
UNO Q connected:

```
ros2 launch wildfire_bringup bringup.launch.py use_sim:=true
```

## Common errors

| Symptom | Cause | Fix |
|---|---|---|
| `get termios attributes error / errno: 25` | the `--dev` path is not a real serial device (e.g. `/dev/null`, missing UNO Q) | check `ls /dev/ttyACM*`, plug the UNO Q over USB-C |
| `Couldn't parse params file: /wildfire_vision// contains repeated forward slash` | stale `params.yaml` with nested `wildfire_vision:` glob — fixed; rebuild `wildfire_bringup` | `colcon build --packages-select wildfire_bringup` |
| Agent runs but no `/ultrasonic/*` topics show up | firmware did not reach `rclc_executor_spin()`; check that the firmware was built and flashed (`pio run -e uno_q_mcu -t upload`) | re-flash and watch the serial monitor |
| `Permission denied: '/dev/ttyACM0'` | user not in the `dialout` group (linux host) or container missing `--device` mapping | `sudo usermod -aG dialout $USER` and re-login, or expose the device into docker |

## Why we build instead of `apt install`

`apt-cache search` on `ros-humble-*` lists `micro-ros-msgs`,
`micro-ros-diagnostic-bridge` and `micro-ros-diagnostic-msgs` but not
`micro-ros-agent`. The Agent is published only as source on
`github.com/micro-ROS/micro-ROS-Agent` and the recommended way to
install it on Humble is the `micro_ros_setup` helper used here.

## File layout produced by setup.sh

```
/home/project/microros_ws/
├── build/
├── install/
│   ├── local_setup.bash          ← sourced from ~/.bashrc
│   └── micro_ros_agent/
│       └── lib/micro_ros_agent/micro_ros_agent   ← the binary
├── log/
└── src/
    ├── micro_ros_setup/           ← helper scripts
    └── uros/
        ├── micro-ROS-Agent/       ← Agent sources
        └── micro_ros_msgs/
```