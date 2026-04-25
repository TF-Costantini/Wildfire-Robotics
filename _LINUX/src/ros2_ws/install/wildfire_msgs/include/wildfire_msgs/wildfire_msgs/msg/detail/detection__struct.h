// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from wildfire_msgs:msg/Detection.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__DETECTION__STRUCT_H_
#define WILDFIRE_MSGS__MSG__DETAIL__DETECTION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/Detection in the package wildfire_msgs.
/**
  * Detection.msg — Dati rilevamento (persona o fuoco) per Wildfire Robotics UGV
 */
typedef struct wildfire_msgs__msg__Detection
{
  bool found;
  float cx;
  float cy;
  float area;
  float img_w;
  float img_h;
  float confidence;
  bool use_confidence;
} wildfire_msgs__msg__Detection;

// Struct for a sequence of wildfire_msgs__msg__Detection.
typedef struct wildfire_msgs__msg__Detection__Sequence
{
  wildfire_msgs__msg__Detection * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} wildfire_msgs__msg__Detection__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // WILDFIRE_MSGS__MSG__DETAIL__DETECTION__STRUCT_H_
