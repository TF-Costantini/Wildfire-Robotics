# Wildfire UGV — host-side test scripts

These scripts test the ROS2 nodes in the running `ros2_dev` container by
publishing synthetic inputs and inspecting the outputs. No hardware needed.

## Prerequisites

1. Container built and running:
   ```bash
   ./rebuild_docker.sh   # first time
   ./start_docker.sh     # subsequent sessions
   ```
2. Workspace built inside the container (handled by `_LINUX/setup.sh`).
3. For `test_07_person_detector.sh`: `pip install ultralytics` inside the
   container.

## Layout

| File                            | What it tests                                     |
|---------------------------------|---------------------------------------------------|
| `_lib.sh`                       | shared helpers (dexec, dexec_bg, dkill)           |
| `test_01_smoke.sh`              | every node starts and stays alive 3s              |
| `test_02_state_machine.sh`      | IDLE -> FOLLOW -> FIRE -> IDLE rotation           |
| `test_03_state_gating.sh`       | mutual exclusion of cmd forwarding per mode       |
| `test_04_follow_controller.sh`  | drive logic with fake person + ultrasonics        |
| `test_05_fire_controller.sh`    | sweep / tracking / lock FSM with fake detections  |
| `test_06_fire_detector.sh`      | HSV pipeline on `test_images/test_fire.jpg`       |
| `test_07_person_detector.sh`    | YOLOv8n on `test_images/test_person.jpg` (slow)   |
| `run_all.sh`                    | run all tests in order                            |

## Running

From the repo root:
```bash
./_LINUX/tests/test_02_state_machine.sh    # one test
./_LINUX/tests/run_all.sh                  # all tests
SKIP_YOLO=1 ./_LINUX/tests/run_all.sh      # skip slow YOLO test
```

Each test prints `[OK]` / `[KO]` lines for its assertions and dumps raw
`ros2 topic echo` output so you can eyeball numerical values.