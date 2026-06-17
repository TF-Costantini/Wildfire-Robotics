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
        # Installa launch/ e config/ nella share directory.
        # Solo file regolari: os.listdir includerebbe __pycache__ (una dir),
        # che data_files non sa copiare → build fallisce.
        (os.path.join('share', package_name, 'launch'),
            [os.path.join('launch', f) for f in os.listdir('launch')
                if os.path.isfile(os.path.join('launch', f))]),
        (os.path.join('share', package_name, 'config'),
            [os.path.join('config', f) for f in os.listdir('config')
                if os.path.isfile(os.path.join('config', f))]),
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
