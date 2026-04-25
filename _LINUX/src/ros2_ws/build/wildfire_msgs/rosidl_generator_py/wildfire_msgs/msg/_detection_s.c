// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from wildfire_msgs:msg/Detection.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_runtime_c/visibility_control.h"
#include "wildfire_msgs/msg/detail/detection__struct.h"
#include "wildfire_msgs/msg/detail/detection__functions.h"


ROSIDL_GENERATOR_C_EXPORT
bool wildfire_msgs__msg__detection__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[39];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp("wildfire_msgs.msg._detection.Detection", full_classname_dest, 38) == 0);
  }
  wildfire_msgs__msg__Detection * ros_message = _ros_message;
  {  // found
    PyObject * field = PyObject_GetAttrString(_pymsg, "found");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->found = (Py_True == field);
    Py_DECREF(field);
  }
  {  // cx
    PyObject * field = PyObject_GetAttrString(_pymsg, "cx");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->cx = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // cy
    PyObject * field = PyObject_GetAttrString(_pymsg, "cy");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->cy = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // area
    PyObject * field = PyObject_GetAttrString(_pymsg, "area");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->area = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // img_w
    PyObject * field = PyObject_GetAttrString(_pymsg, "img_w");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->img_w = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // img_h
    PyObject * field = PyObject_GetAttrString(_pymsg, "img_h");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->img_h = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // confidence
    PyObject * field = PyObject_GetAttrString(_pymsg, "confidence");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->confidence = (float)PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // use_confidence
    PyObject * field = PyObject_GetAttrString(_pymsg, "use_confidence");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->use_confidence = (Py_True == field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * wildfire_msgs__msg__detection__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of Detection */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("wildfire_msgs.msg._detection");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "Detection");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  wildfire_msgs__msg__Detection * ros_message = (wildfire_msgs__msg__Detection *)raw_ros_message;
  {  // found
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->found ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "found", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // cx
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->cx);
    {
      int rc = PyObject_SetAttrString(_pymessage, "cx", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // cy
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->cy);
    {
      int rc = PyObject_SetAttrString(_pymessage, "cy", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // area
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->area);
    {
      int rc = PyObject_SetAttrString(_pymessage, "area", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // img_w
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->img_w);
    {
      int rc = PyObject_SetAttrString(_pymessage, "img_w", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // img_h
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->img_h);
    {
      int rc = PyObject_SetAttrString(_pymessage, "img_h", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // confidence
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->confidence);
    {
      int rc = PyObject_SetAttrString(_pymessage, "confidence", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // use_confidence
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->use_confidence ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "use_confidence", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
