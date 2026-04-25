# generated from rosidl_generator_py/resource/_idl.py.em
# with input from wildfire_msgs:msg/Detection.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_Detection(type):
    """Metaclass of message 'Detection'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('wildfire_msgs')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'wildfire_msgs.msg.Detection')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__detection
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__detection
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__detection
            cls._TYPE_SUPPORT = module.type_support_msg__msg__detection
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__detection

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class Detection(metaclass=Metaclass_Detection):
    """Message class 'Detection'."""

    __slots__ = [
        '_found',
        '_cx',
        '_cy',
        '_area',
        '_img_w',
        '_img_h',
        '_confidence',
        '_use_confidence',
    ]

    _fields_and_field_types = {
        'found': 'boolean',
        'cx': 'float',
        'cy': 'float',
        'area': 'float',
        'img_w': 'float',
        'img_h': 'float',
        'confidence': 'float',
        'use_confidence': 'boolean',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.found = kwargs.get('found', bool())
        self.cx = kwargs.get('cx', float())
        self.cy = kwargs.get('cy', float())
        self.area = kwargs.get('area', float())
        self.img_w = kwargs.get('img_w', float())
        self.img_h = kwargs.get('img_h', float())
        self.confidence = kwargs.get('confidence', float())
        self.use_confidence = kwargs.get('use_confidence', bool())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.found != other.found:
            return False
        if self.cx != other.cx:
            return False
        if self.cy != other.cy:
            return False
        if self.area != other.area:
            return False
        if self.img_w != other.img_w:
            return False
        if self.img_h != other.img_h:
            return False
        if self.confidence != other.confidence:
            return False
        if self.use_confidence != other.use_confidence:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def found(self):
        """Message field 'found'."""
        return self._found

    @found.setter
    def found(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'found' field must be of type 'bool'"
        self._found = value

    @builtins.property
    def cx(self):
        """Message field 'cx'."""
        return self._cx

    @cx.setter
    def cx(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'cx' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'cx' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._cx = value

    @builtins.property
    def cy(self):
        """Message field 'cy'."""
        return self._cy

    @cy.setter
    def cy(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'cy' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'cy' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._cy = value

    @builtins.property
    def area(self):
        """Message field 'area'."""
        return self._area

    @area.setter
    def area(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'area' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'area' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._area = value

    @builtins.property
    def img_w(self):
        """Message field 'img_w'."""
        return self._img_w

    @img_w.setter
    def img_w(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'img_w' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'img_w' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._img_w = value

    @builtins.property
    def img_h(self):
        """Message field 'img_h'."""
        return self._img_h

    @img_h.setter
    def img_h(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'img_h' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'img_h' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._img_h = value

    @builtins.property
    def confidence(self):
        """Message field 'confidence'."""
        return self._confidence

    @confidence.setter
    def confidence(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'confidence' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'confidence' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._confidence = value

    @builtins.property
    def use_confidence(self):
        """Message field 'use_confidence'."""
        return self._use_confidence

    @use_confidence.setter
    def use_confidence(self, value):
        if __debug__:
            assert \
                isinstance(value, bool), \
                "The 'use_confidence' field must be of type 'bool'"
        self._use_confidence = value
