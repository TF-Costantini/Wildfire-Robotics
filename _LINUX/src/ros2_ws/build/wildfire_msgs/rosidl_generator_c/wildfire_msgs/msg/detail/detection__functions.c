// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from wildfire_msgs:msg/Detection.idl
// generated code does not contain a copyright notice
#include "wildfire_msgs/msg/detail/detection__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


bool
wildfire_msgs__msg__Detection__init(wildfire_msgs__msg__Detection * msg)
{
  if (!msg) {
    return false;
  }
  // found
  // cx
  // cy
  // area
  // img_w
  // img_h
  // confidence
  // use_confidence
  return true;
}

void
wildfire_msgs__msg__Detection__fini(wildfire_msgs__msg__Detection * msg)
{
  if (!msg) {
    return;
  }
  // found
  // cx
  // cy
  // area
  // img_w
  // img_h
  // confidence
  // use_confidence
}

bool
wildfire_msgs__msg__Detection__are_equal(const wildfire_msgs__msg__Detection * lhs, const wildfire_msgs__msg__Detection * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // found
  if (lhs->found != rhs->found) {
    return false;
  }
  // cx
  if (lhs->cx != rhs->cx) {
    return false;
  }
  // cy
  if (lhs->cy != rhs->cy) {
    return false;
  }
  // area
  if (lhs->area != rhs->area) {
    return false;
  }
  // img_w
  if (lhs->img_w != rhs->img_w) {
    return false;
  }
  // img_h
  if (lhs->img_h != rhs->img_h) {
    return false;
  }
  // confidence
  if (lhs->confidence != rhs->confidence) {
    return false;
  }
  // use_confidence
  if (lhs->use_confidence != rhs->use_confidence) {
    return false;
  }
  return true;
}

bool
wildfire_msgs__msg__Detection__copy(
  const wildfire_msgs__msg__Detection * input,
  wildfire_msgs__msg__Detection * output)
{
  if (!input || !output) {
    return false;
  }
  // found
  output->found = input->found;
  // cx
  output->cx = input->cx;
  // cy
  output->cy = input->cy;
  // area
  output->area = input->area;
  // img_w
  output->img_w = input->img_w;
  // img_h
  output->img_h = input->img_h;
  // confidence
  output->confidence = input->confidence;
  // use_confidence
  output->use_confidence = input->use_confidence;
  return true;
}

wildfire_msgs__msg__Detection *
wildfire_msgs__msg__Detection__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__Detection * msg = (wildfire_msgs__msg__Detection *)allocator.allocate(sizeof(wildfire_msgs__msg__Detection), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(wildfire_msgs__msg__Detection));
  bool success = wildfire_msgs__msg__Detection__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
wildfire_msgs__msg__Detection__destroy(wildfire_msgs__msg__Detection * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    wildfire_msgs__msg__Detection__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
wildfire_msgs__msg__Detection__Sequence__init(wildfire_msgs__msg__Detection__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__Detection * data = NULL;

  if (size) {
    data = (wildfire_msgs__msg__Detection *)allocator.zero_allocate(size, sizeof(wildfire_msgs__msg__Detection), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = wildfire_msgs__msg__Detection__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        wildfire_msgs__msg__Detection__fini(&data[i - 1]);
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
wildfire_msgs__msg__Detection__Sequence__fini(wildfire_msgs__msg__Detection__Sequence * array)
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
      wildfire_msgs__msg__Detection__fini(&array->data[i]);
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

wildfire_msgs__msg__Detection__Sequence *
wildfire_msgs__msg__Detection__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  wildfire_msgs__msg__Detection__Sequence * array = (wildfire_msgs__msg__Detection__Sequence *)allocator.allocate(sizeof(wildfire_msgs__msg__Detection__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = wildfire_msgs__msg__Detection__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
wildfire_msgs__msg__Detection__Sequence__destroy(wildfire_msgs__msg__Detection__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    wildfire_msgs__msg__Detection__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
wildfire_msgs__msg__Detection__Sequence__are_equal(const wildfire_msgs__msg__Detection__Sequence * lhs, const wildfire_msgs__msg__Detection__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!wildfire_msgs__msg__Detection__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
wildfire_msgs__msg__Detection__Sequence__copy(
  const wildfire_msgs__msg__Detection__Sequence * input,
  wildfire_msgs__msg__Detection__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(wildfire_msgs__msg__Detection);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    wildfire_msgs__msg__Detection * data =
      (wildfire_msgs__msg__Detection *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!wildfire_msgs__msg__Detection__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          wildfire_msgs__msg__Detection__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!wildfire_msgs__msg__Detection__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
