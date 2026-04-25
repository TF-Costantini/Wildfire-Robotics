"""
bringup.launch.py — Wildfire Robotics UGV
Launch file principale per avviare tutti i nodi del sistema UGV.

Nodi avviati (in ordine):
  1. camera_node         (wildfire_vision)
  2. person_detector_node (wildfire_vision)
  3. fire_detector_node  (wildfire_vision)
  4. state_machine_node  (wildfire_control)
  5. follow_controller_node (wildfire_control)
  6. fire_controller_node   (wildfire_control)

I parametri sono caricati da config/params.yaml.
"""

import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    pkg_bringup = get_package_share_directory('wildfire_bringup')
    config_file = os.path.join(pkg_bringup, 'config', 'params.yaml')

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
    # Launch description
    # ================================================================
    ld = LaunchDescription([
        camera_node,
        person_detector_node,
        fire_detector_node,
        state_machine_node,
        follow_controller_node,
        fire_controller_node,
    ])

    return ld
