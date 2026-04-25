// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from wildfire_msgs:msg/ButtonEvent.idl
// generated code does not contain a copyright notice
#include "wildfire_msgs/msg/detail/button_event__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
wildfire_msgs__msg__ButtonEvent__init(wildfire_msgs__msg__ButtonEvent * msg)
{
  if (!msg) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    wildfire_msgs__msg__ButtonEvent__fini(msg);
    return false;
  }
  // kind
  return true;
}

void
wildfire_msgs__msg__ButtonEvent__fini(wildfire_msgs__msg__ButtonEvent * msg)
{
  if (!msg) {
    return;
  }
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
  // kind
}

bool
wildfire_msgs__msg__ButtonEvent__are_equal(const wildfire_msgs__msg__ButtonEvent * lhs, const wildfire_msgs__msg__ButtonEvent * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  // kind
  if (lhs->kind != rhs->kind) {
    return false;
  }
  return true;
}

bool
wildfire_msgs__msg__ButtonEvent__copy(
  const wildfire_msgs__msg__ButtonEvent * input,
  wildfire_msgs__msg__ButtonEvent * output)
{
  if (!input || !output) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  // kind
  output->kind = input->kind;
  return true;
}

wildfire_msgs__msg__ButtonEvent *
wildfire_msgs__msg__ButtonEvent__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__ButtonEvent * msg = (wildfire_msgs__msg__ButtonEvent *)allocator.allocate(sizeof(wildfire_msgs__msg__ButtonEvent), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(wildfire_msgs__msg__ButtonEvent));
  bool success = wildfire_msgs__msg__ButtonEvent__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
wildfire_msgs__msg__ButtonEvent__destroy(wildfire_msgs__msg__ButtonEvent * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    wildfire_msgs__msg__ButtonEvent__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
wildfire_msgs__msg__ButtonEvent__Sequence__init(wildfire_msgs__msg__ButtonEvent__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__ButtonEvent * data = NULL;

  if (size) {
    data = (wildfire_msgs__msg__ButtonEvent *)allocator.zero_allocate(size, sizeof(wildfire_msgs__msg__ButtonEvent), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = wildfire_msgs__msg__ButtonEvent__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        wildfire_msgs__msg__ButtonEvent__fini(&data[i - 1]);
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
wildfire_msgs__msg__ButtonEvent__Sequence__fini(wildfire_msgs__msg__ButtonEvent__Sequence * array)
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
      wildfire_msgs__msg__ButtonEvent__fini(&array->data[i]);
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

wildfire_msgs__msg__ButtonEvent__Sequence *
wildfire_msgs__msg__ButtonEvent__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__ButtonEvent__Sequence * array = (wildfire_msgs__msg__ButtonEvent__Sequence *)allocator.allocate(sizeof(wildfire_msgs__msg__ButtonEvent__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = wildfire_msgs__msg__ButtonEvent__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
wildfire_msgs__msg__ButtonEvent__Sequence__destroy(wildfire_msgs__msg__ButtonEvent__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    wildfire_msgs__msg__ButtonEvent__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
wildfire_msgs__msg__ButtonEvent__Sequence__are_equal(const wildfire_msgs__msg__ButtonEvent__Sequence * lhs, const wildfire_msgs__msg__ButtonEvent__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!wildfire_msgs__msg__ButtonEvent__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
wildfire_msgs__msg__ButtonEvent__Sequence__copy(
  const wildfire_msgs__msg__ButtonEvent__Sequence * input,
  wildfire_msgs__msg__ButtonEvent__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(wildfire_msgs__msg__ButtonEvent);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    wildfire_msgs__msg__ButtonEvent * data =
      (wildfire_msgs__msg__ButtonEvent *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!wildfire_msgs__msg__ButtonEvent__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          wildfire_msgs__msg__ButtonEvent__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!wildfire_msgs__msg__ButtonEvent__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
