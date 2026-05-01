from setuptools import setup

package_name = 'wildfire_control'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Wildfire Robotics Team',
    maintainer_email='team@wildfire-robotics.local',
    description='Control nodes for Wildfire Robotics UGV',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'state_machine_node = wildfire_control.state_machine_node:main',
            'follow_controller_node = wildfire_control.follow_controller_node:main',
            'fire_controller_node = wildfire_control.fire_controller_node:main',
            'comm_bridge_node = wildfire_control.comm_bridge_node:main',
            'mcu_sim_node = wildfire_control.mcu_sim_node:main',
        ],
    },
)
