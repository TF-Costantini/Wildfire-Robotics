// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from wildfire_msgs:msg/ButtonEvent.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__STRUCT_H_
#define WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Constant 'PRESS'.
enum
{
  wildfire_msgs__msg__ButtonEvent__PRESS = 0
};

/// Constant 'RELEASE'.
enum
{
  wildfire_msgs__msg__ButtonEvent__RELEASE = 1
};

/// Constant 'LONG_PRESS'.
enum
{
  wildfire_msgs__msg__ButtonEvent__LONG_PRESS = 2
};

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in msg/ButtonEvent in the package wildfire_msgs.
/**
  * ButtonEvent.msg — Eventi del pulsante per Wildfire Robotics UGV
 */
typedef struct wildfire_msgs__msg__ButtonEvent
{
  builtin_interfaces__msg__Time stamp;
  uint8_t kind;
} wildfire_msgs__msg__ButtonEvent;

// Struct for a sequence of wildfire_msgs__msg__ButtonEvent.
typedef struct wildfire_msgs__msg__ButtonEvent__Sequence
{
  wildfire_msgs__msg__ButtonEvent * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} wildfire_msgs__msg__ButtonEvent__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__STRUCT_H_
