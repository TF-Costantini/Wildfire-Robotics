from setuptools import setup

package_name = 'wildfire_vision'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools', 'ultralytics'],
    zip_safe=True,
    maintainer='Wildfire Robotics Team',
    maintainer_email='team@wildfire-robotics.local',
    description='Vision processing nodes for Wildfire Robotics UGV',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'camera_node = wildfire_vision.camera_node:main',
            'fake_camera_node = wildfire_vision.fake_camera_node:main',
            'fire_detector_node = wildfire_vision.fire_detector_node:main',
            'person_detector_node = wildfire_vision.person_detector_node:main',
        ],
    },
)
