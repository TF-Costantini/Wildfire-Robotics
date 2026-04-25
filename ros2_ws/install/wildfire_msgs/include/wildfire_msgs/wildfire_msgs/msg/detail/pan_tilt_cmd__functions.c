// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from wildfire_msgs:msg/PanTiltCmd.idl
// generated code does not contain a copyright notice
#include "wildfire_msgs/msg/detail/pan_tilt_cmd__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
wildfire_msgs__msg__PanTiltCmd__init(wildfire_msgs__msg__PanTiltCmd * msg)
{
  if (!msg) {
    return false;
  }
  // pan_deg
  // tilt_deg
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    wildfire_msgs__msg__PanTiltCmd__fini(msg);
    return false;
  }
  return true;
}

void
wildfire_msgs__msg__PanTiltCmd__fini(wildfire_msgs__msg__PanTiltCmd * msg)
{
  if (!msg) {
    return;
  }
  // pan_deg
  // tilt_deg
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
}

bool
wildfire_msgs__msg__PanTiltCmd__are_equal(const wildfire_msgs__msg__PanTiltCmd * lhs, const wildfire_msgs__msg__PanTiltCmd * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // pan_deg
  if (lhs->pan_deg != rhs->pan_deg) {
    return false;
  }
  // tilt_deg
  if (lhs->tilt_deg != rhs->tilt_deg) {
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
wildfire_msgs__msg__PanTiltCmd__copy(
  const wildfire_msgs__msg__PanTiltCmd * input,
  wildfire_msgs__msg__PanTiltCmd * output)
{
  if (!input || !output) {
    return false;
  }
  // pan_deg
  output->pan_deg = input->pan_deg;
  // tilt_deg
  output->tilt_deg = input->tilt_deg;
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  return true;
}

wildfire_msgs__msg__PanTiltCmd *
wildfire_msgs__msg__PanTiltCmd__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__PanTiltCmd * msg = (wildfire_msgs__msg__PanTiltCmd *)allocator.allocate(sizeof(wildfire_msgs__msg__PanTiltCmd), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(wildfire_msgs__msg__PanTiltCmd));
  bool success = wildfire_msgs__msg__PanTiltCmd__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
wildfire_msgs__msg__PanTiltCmd__destroy(wildfire_msgs__msg__PanTiltCmd * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    wildfire_msgs__msg__PanTiltCmd__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
wildfire_msgs__msg__PanTiltCmd__Sequence__init(wildfire_msgs__msg__PanTiltCmd__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__PanTiltCmd * data = NULL;

  if (size) {
    data = (wildfire_msgs__msg__PanTiltCmd *)allocator.zero_allocate(size, sizeof(wildfire_msgs__msg__PanTiltCmd), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = wildfire_msgs__msg__PanTiltCmd__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        wildfire_msgs__msg__PanTiltCmd__fini(&data[i - 1]);
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
wildfire_msgs__msg__PanTiltCmd__Sequence__fini(wildfire_msgs__msg__PanTiltCmd__Sequence * array)
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
      wildfire_msgs__msg__PanTiltCmd__fini(&array->data[i]);
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

wildfire_msgs__msg__PanTiltCmd__Sequence *
wildfire_msgs__msg__PanTiltCmd__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__PanTiltCmd__Sequence * array = (wildfire_msgs__msg__PanTiltCmd__Sequence *)allocator.allocate(sizeof(wildfire_msgs__msg__PanTiltCmd__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = wildfire_msgs__msg__PanTiltCmd__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
wildfire_msgs__msg__PanTiltCmd__Sequence__destroy(wildfire_msgs__msg__PanTiltCmd__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    wildfire_msgs__msg__PanTiltCmd__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
wildfire_msgs__msg__PanTiltCmd__Sequence__are_equal(const wildfire_msgs__msg__PanTiltCmd__Sequence * lhs, const wildfire_msgs__msg__PanTiltCmd__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!wildfire_msgs__msg__PanTiltCmd__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
wildfire_msgs__msg__PanTiltCmd__Sequence__copy(
  const wildfire_msgs__msg__PanTiltCmd__Sequence * input,
  wildfire_msgs__msg__PanTiltCmd__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(wildfire_msgs__msg__PanTiltCmd);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    wildfire_msgs__msg__PanTiltCmd * data =
      (wildfire_msgs__msg__PanTiltCmd *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!wildfire_msgs__msg__PanTiltCmd__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          wildfire_msgs__msg__PanTiltCmd__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!wildfire_msgs__msg__PanTiltCmd__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
