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

In modalita simulata (use_sim:=true) viene avviato anche
mcu_sim_node (wildfire_control), che impersona il firmware MCU
sul grafo ROS2: pubblica /ultrasonic/* e /button_event, sottoscrive
i topic /cmd_*_mcu. Utile per testare lo stack senza l'Arduino UNO Q.

  ros2 launch wildfire_bringup bringup.launch.py            # default, hardware reale
  ros2 launch wildfire_bringup bringup.launch.py use_sim:=true   # senza hardware

I parametri sono caricati da config/params.yaml.
"""

import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
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
    # Launch description
    # ================================================================
    return LaunchDescription([
        use_sim_arg,
        camera_node,
        person_detector_node,
        fire_detector_node,
        state_machine_node,
        follow_controller_node,
        fire_controller_node,
        mcu_sim_node,
    ])
