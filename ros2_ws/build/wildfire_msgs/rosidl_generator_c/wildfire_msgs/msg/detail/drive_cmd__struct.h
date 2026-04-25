// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from wildfire_msgs:msg/DriveCmd.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__STRUCT_H_
#define WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__STRUCT_H_

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

/// Struct defined in msg/DriveCmd in the package wildfire_msgs.
/**
  * DriveCmd.msg — Comando driving differenziale per Wildfire Robotics UGV
  * left/right: -1.0 (indietro max) .. 1.0 (avanti max)
 */
typedef struct wildfire_msgs__msg__DriveCmd
{
  float left;
  float right;
  builtin_interfaces__msg__Time stamp;
} wildfire_msgs__msg__DriveCmd;

// Struct for a sequence of wildfire_msgs__msg__DriveCmd.
typedef struct wildfire_msgs__msg__DriveCmd__Sequence
{
  wildfire_msgs__msg__DriveCmd * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} wildfire_msgs__msg__DriveCmd__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__STRUCT_H_
