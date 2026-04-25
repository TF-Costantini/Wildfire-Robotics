// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from wildfire_msgs:msg/PanTiltCmd.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__FUNCTIONS_H_
#define WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "wildfire_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "wildfire_msgs/msg/detail/pan_tilt_cmd__struct.h"

/// Initialize msg/PanTiltCmd message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * wildfire_msgs__msg__PanTiltCmd
 * )) before or use
 * wildfire_msgs__msg__PanTiltCmd__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
bool
wildfire_msgs__msg__PanTiltCmd__init(wildfire_msgs__msg__PanTiltCmd * msg);

/// Finalize msg/PanTiltCmd message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
void
wildfire_msgs__msg__PanTiltCmd__fini(wildfire_msgs__msg__PanTiltCmd * msg);

/// Create msg/PanTiltCmd message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * wildfire_msgs__msg__PanTiltCmd__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
wildfire_msgs__msg__PanTiltCmd *
wildfire_msgs__msg__PanTiltCmd__create();

/// Destroy msg/PanTiltCmd message.
/**
 * It calls
 * wildfire_msgs__msg__PanTiltCmd__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
void
wildfire_msgs__msg__PanTiltCmd__destroy(wildfire_msgs__msg__PanTiltCmd * msg);

/// Check for msg/PanTiltCmd message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
bool
wildfire_msgs__msg__PanTiltCmd__are_equal(const wildfire_msgs__msg__PanTiltCmd * lhs, const wildfire_msgs__msg__PanTiltCmd * rhs);

/// Copy a msg/PanTiltCmd message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
bool
wildfire_msgs__msg__PanTiltCmd__copy(
  const wildfire_msgs__msg__PanTiltCmd * input,
  wildfire_msgs__msg__PanTiltCmd * output);

/// Initialize array of msg/PanTiltCmd messages.
/**
 * It allocates the memory for the number of elements and calls
 * wildfire_msgs__msg__PanTiltCmd__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
bool
wildfire_msgs__msg__PanTiltCmd__Sequence__init(wildfire_msgs__msg__PanTiltCmd__Sequence * array, size_t size);

/// Finalize array of msg/PanTiltCmd messages.
/**
 * It calls
 * wildfire_msgs__msg__PanTiltCmd__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
void
wildfire_msgs__msg__PanTiltCmd__Sequence__fini(wildfire_msgs__msg__PanTiltCmd__Sequence * array);

/// Create array of msg/PanTiltCmd messages.
/**
 * It allocates the memory for the array and calls
 * wildfire_msgs__msg__PanTiltCmd__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
wildfire_msgs__msg__PanTiltCmd__Sequence *
wildfire_msgs__msg__PanTiltCmd__Sequence__create(size_t size);

/// Destroy array of msg/PanTiltCmd messages.
/**
 * It calls
 * wildfire_msgs__msg__PanTiltCmd__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
void
wildfire_msgs__msg__PanTiltCmd__Sequence__destroy(wildfire_msgs__msg__PanTiltCmd__Sequence * array);

/// Check for msg/PanTiltCmd message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
bool
wildfire_msgs__msg__PanTiltCmd__Sequence__are_equal(const wildfire_msgs__msg__PanTiltCmd__Sequence * lhs, const wildfire_msgs__msg__PanTiltCmd__Sequence * rhs);

/// Copy an array of msg/PanTiltCmd messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_wildfire_msgs
bool
wildfire_msgs__msg__PanTiltCmd__Sequence__copy(
  const wildfire_msgs__msg__PanTiltCmd__Sequence * input,
  wildfire_msgs__msg__PanTiltCmd__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__FUNCTIONS_H_
