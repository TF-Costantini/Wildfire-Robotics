# wildfire_vision/__init__.py
# Autore: Wildfire Robotics Team
#
# TODO: Implementare il package wildfire_vision
# TODO: Importare tutti i moduli nodes disponibili

from .camera_node import CameraNode
from .person_detector_node import PersonDetectorNode
from .fire_detector_node import FireDetectorNode

__all__ = ['CameraNode', 'PersonDetectorNode', 'FireDetectorNode']
