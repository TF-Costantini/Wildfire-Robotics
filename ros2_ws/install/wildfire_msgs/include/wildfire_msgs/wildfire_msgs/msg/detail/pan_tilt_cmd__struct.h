// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from wildfire_msgs:msg/PanTiltCmd.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__STRUCT_H_
#define WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in msg/PanTiltCmd in the package wildfire_msgs.
/**
  * PanTiltCmd.msg — Comando pan-tilt per Wildfire Robotics UGV
  * pan_deg: -60.0 .. +60.0, tilt_deg: 0.0 .. +30.0
 */
typedef struct wildfire_msgs__msg__PanTiltCmd
{
  float pan_deg;
  float tilt_deg;
  builtin_interfaces__msg__Time stamp;
} wildfire_msgs__msg__PanTiltCmd;

// Struct for a sequence of wildfire_msgs__msg__PanTiltCmd.
typedef struct wildfire_msgs__msg__PanTiltCmd__Sequence
{
  wildfire_msgs__msg__PanTiltCmd * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} wildfire_msgs__msg__PanTiltCmd__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__STRUCT_H_
