from setuptools import find_packages
from setuptools import setup

setup(
    name='wildfire_msgs',
    version='0.1.0',
    packages=find_packages(
        include=('wildfire_msgs', 'wildfire_msgs.*')),
)
