from setuptools import setup
import os

package_name = 'wildfire_bringup'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        # Installa launch/ e config/ nella share directory
        (os.path.join('share', package_name, 'launch'),
            [os.path.join('launch', f) for f in os.listdir('launch')]),
        (os.path.join('share', package_name, 'config'),
            [os.path.join('config', f) for f in os.listdir('config')]),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Wildfire Robotics Team',
    maintainer_email='team@wildfire-robotics.local',
    description='Launch files and configuration for Wildfire Robotics UGV',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        ],
    },
)
