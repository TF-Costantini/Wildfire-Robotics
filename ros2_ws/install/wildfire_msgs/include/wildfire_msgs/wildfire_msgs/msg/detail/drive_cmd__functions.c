// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from wildfire_msgs:msg/DriveCmd.idl
// generated code does not contain a copyright notice
#include "wildfire_msgs/msg/detail/drive_cmd__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
wildfire_msgs__msg__DriveCmd__init(wildfire_msgs__msg__DriveCmd * msg)
{
  if (!msg) {
    return false;
  }
  // left
  // right
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    wildfire_msgs__msg__DriveCmd__fini(msg);
    return false;
  }
  return true;
}

void
wildfire_msgs__msg__DriveCmd__fini(wildfire_msgs__msg__DriveCmd * msg)
{
  if (!msg) {
    return;
  }
  // left
  // right
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
}

bool
wildfire_msgs__msg__DriveCmd__are_equal(const wildfire_msgs__msg__DriveCmd * lhs, const wildfire_msgs__msg__DriveCmd * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // left
  if (lhs->left != rhs->left) {
    return false;
  }
  // right
  if (lhs->right != rhs->right) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  return true;
}

bool
wildfire_msgs__msg__DriveCmd__copy(
  const wildfire_msgs__msg__DriveCmd * input,
  wildfire_msgs__msg__DriveCmd * output)
{
  if (!input || !output) {
    return false;
  }
  // left
  output->left = input->left;
  // right
  output->right = input->right;
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  return true;
}

wildfire_msgs__msg__DriveCmd *
wildfire_msgs__msg__DriveCmd__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__DriveCmd * msg = (wildfire_msgs__msg__DriveCmd *)allocator.allocate(sizeof(wildfire_msgs__msg__DriveCmd), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(wildfire_msgs__msg__DriveCmd));
  bool success = wildfire_msgs__msg__DriveCmd__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
wildfire_msgs__msg__DriveCmd__destroy(wildfire_msgs__msg__DriveCmd * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    wildfire_msgs__msg__DriveCmd__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
wildfire_msgs__msg__DriveCmd__Sequence__init(wildfire_msgs__msg__DriveCmd__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__DriveCmd * data = NULL;

  if (size) {
    data = (wildfire_msgs__msg__DriveCmd *)allocator.zero_allocate(size, sizeof(wildfire_msgs__msg__DriveCmd), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = wildfire_msgs__msg__DriveCmd__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        wildfire_msgs__msg__DriveCmd__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
wildfire_msgs__msg__DriveCmd__Sequence__fini(wildfire_msgs__msg__DriveCmd__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      wildfire_msgs__msg__DriveCmd__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

wildfire_msgs__msg__DriveCmd__Sequence *
wildfire_msgs__msg__DriveCmd__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__DriveCmd__Sequence * array = (wildfire_msgs__msg__DriveCmd__Sequence *)allocator.allocate(sizeof(wildfire_msgs__msg__DriveCmd__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = wildfire_msgs__msg__DriveCmd__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
wildfire_msgs__msg__DriveCmd__Sequence__destroy(wildfire_msgs__msg__DriveCmd__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    wildfire_msgs__msg__DriveCmd__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
wildfire_msgs__msg__DriveCmd__Sequence__are_equal(const wildfire_msgs__msg__DriveCmd__Sequence * lhs, const wildfire_msgs__msg__DriveCmd__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!wildfire_msgs__msg__DriveCmd__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
wildfire_msgs__msg__DriveCmd__Sequence__copy(
  const wildfire_msgs__msg__DriveCmd__Sequence * input,
  wildfire_msgs__msg__DriveCmd__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(wildfire_msgs__msg__DriveCmd);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    wildfire_msgs__msg__DriveCmd * data =
      (wildfire_msgs__msg__DriveCmd *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!wildfire_msgs__msg__DriveCmd__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          wildfire_msgs__msg__DriveCmd__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!wildfire_msgs__msg__DriveCmd__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
