"""
bringup.launch.py — Wildfire Robotics UGV
Launch file principale per avviare tutti i nodi del sistema UGV.

Nodi avviati (in ordine):
  1. camera_node          (wildfire_vision)
  2. person_detector_node (wildfire_vision)
  3. fire_detector_node   (wildfire_vision)
  4. state_machine_node   (wildfire_control)
  5. follow_controller_node (wildfire_control)
  6. fire_controller_node   (wildfire_control)

Modalita HARDWARE (default, use_sim:=false):
  Viene avviato il micro_ros_agent (pacchetto micro_ros_agent) sulla
  porta seriale specificata da agent_dev (default /dev/ttyACM0). L'agent
  fa da bridge fra il firmware micro-ROS sull'UNO Q (STM32U585) e il
  grafo ROS2: senza di lui i topic /ultrasonic/*, /button_event,
  /cmd_*_mcu non passano fra MCU e MPU.

Modalita SIMULATA (use_sim:=true):
  Niente agent reale. Viene avviato mcu_sim_node (wildfire_control) che
  impersona il firmware sul grafo ROS2: pubblica /ultrasonic/* e
  /button_event, sottoscrive i topic /cmd_*_mcu. Utile per testare lo
  stack senza l'Arduino UNO Q collegato.

Esempi:
  # Hardware reale (UNO Q via ST-LINK su /dev/ttyACM0)
  ros2 launch wildfire_bringup bringup.launch.py

  # Senza hardware (sim)
  ros2 launch wildfire_bringup bringup.launch.py use_sim:=true

  # Override seriale dell'agent
  ros2 launch wildfire_bringup bringup.launch.py agent_dev:=/dev/ttyUSB0 agent_baud:=115200

I parametri sono caricati da config/params.yaml.
"""

import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg_bringup = get_package_share_directory('wildfire_bringup')
    config_file = os.path.join(pkg_bringup, 'config', 'params.yaml')

    use_sim_arg = DeclareLaunchArgument(
        'use_sim',
        default_value='false',
        description='If true, launch mcu_sim_node instead of expecting '
                    'the real micro-ROS firmware on /dev/tty*.'
    )
    use_sim = LaunchConfiguration('use_sim')

    agent_dev_arg = DeclareLaunchArgument(
        'agent_dev',
        default_value='/dev/ttyACM0',
        description='Serial device for the micro_ros_agent transport. '
                    'On UNO Q the ST-LINK CDC enumerates here.'
    )
    agent_baud_arg = DeclareLaunchArgument(
        'agent_baud',
        default_value='115200',
        description='Baud rate of the micro_ros_agent serial transport.'
    )
    agent_dev  = LaunchConfiguration('agent_dev')
    agent_baud = LaunchConfiguration('agent_baud')

    # ================================================================
    # 1. VISION — camera
    # ================================================================
    camera_node = Node(
        package='wildfire_vision',
        executable='camera_node',
        name='camera_node',
        output='screen',
        parameters=[config_file],
    )

    # ================================================================
    # 2. VISION — person detector
    # ================================================================
    person_detector_node = Node(
        package='wildfire_vision',
        executable='person_detector_node',
        name='person_detector_node',
        output='screen',
        parameters=[config_file],
    )

    # ================================================================
    # 3. VISION — fire detector
    # ================================================================
    fire_detector_node = Node(
        package='wildfire_vision',
        executable='fire_detector_node',
        name='fire_detector_node',
        output='screen',
        parameters=[config_file],
    )

    # ================================================================
    # 4. CONTROL — state machine (gating centrale)
    # ================================================================
    state_machine_node = Node(
        package='wildfire_control',
        executable='state_machine_node',
        name='state_machine_node',
        output='screen',
        parameters=[config_file],
    )

    # ================================================================
    # 5. CONTROL — follow controller
    # ================================================================
    follow_controller_node = Node(
        package='wildfire_control',
        executable='follow_controller_node',
        name='follow_controller_node',
        output='screen',
        parameters=[config_file],
    )

    # ================================================================
    # 6. CONTROL — fire controller
    # ================================================================
    fire_controller_node = Node(
        package='wildfire_control',
        executable='fire_controller_node',
        name='fire_controller_node',
        output='screen',
        parameters=[config_file],
    )

    # ================================================================
    # 7. SIM — MCU simulator (only when use_sim:=true)
    # ================================================================
    mcu_sim_node = Node(
        package='wildfire_control',
        executable='mcu_sim_node',
        name='mcu_sim_node',
        output='screen',
        parameters=[config_file],
        condition=IfCondition(use_sim),
    )

    # ================================================================
    # 8. HARDWARE — micro-ROS Agent (only when use_sim:=false)
    # ----------------------------------------------------------------
    # Bridge fra il firmware micro-ROS sull'UNO Q (STM32U585) e il
    # grafo ROS2 dell'MPU. Il pacchetto deriva da
    # github.com/micro-ROS/micro-ROS-Agent ed e' installato da setup.sh
    # in /home/project/microros_ws/install. La sessione deve quindi
    # avere quel local_setup.bash sourcato (setup.sh aggiunge la riga
    # in ~/.bashrc).
    # ================================================================
    micro_ros_agent = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='micro_ros_agent',
        output='screen',
        arguments=['serial', '--dev', agent_dev, '-b', agent_baud],
        condition=UnlessCondition(use_sim),
    )

    # ================================================================
    # Launch description
    # ================================================================
    return LaunchDescription([
        use_sim_arg,
        agent_dev_arg,
        agent_baud_arg,
        camera_node,
        person_detector_node,
        fire_detector_node,
        state_machine_node,
        follow_controller_node,
        fire_controller_node,
        mcu_sim_node,
        micro_ros_agent,
    ])
