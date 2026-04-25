// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from wildfire_msgs:msg/Mode.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__MODE__STRUCT_H_
#define WILDFIRE_MSGS__MSG__DETAIL__MODE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Constant 'IDLE'.
enum
{
  wildfire_msgs__msg__Mode__IDLE = 0
};

/// Constant 'FOLLOW'.
enum
{
  wildfire_msgs__msg__Mode__FOLLOW = 1
};

/// Constant 'FIRE'.
enum
{
  wildfire_msgs__msg__Mode__FIRE = 2
};

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in msg/Mode in the package wildfire_msgs.
/**
  * Mode.msg — Modalità operative del robot per Wildfire Robotics UGV
 */
typedef struct wildfire_msgs__msg__Mode
{
  uint8_t mode;
  builtin_interfaces__msg__Time stamp;
} wildfire_msgs__msg__Mode;

// Struct for a sequence of wildfire_msgs__msg__Mode.
typedef struct wildfire_msgs__msg__Mode__Sequence
{
  wildfire_msgs__msg__Mode * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} wildfire_msgs__msg__Mode__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // WILDFIRE_MSGS__MSG__DETAIL__MODE__STRUCT_H_
