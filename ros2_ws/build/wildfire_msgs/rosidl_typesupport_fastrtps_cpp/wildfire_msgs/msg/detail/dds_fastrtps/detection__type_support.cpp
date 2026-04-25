// generated from rosidl_typesupport_fastrtps_cpp/resource/idl__type_support.cpp.em
// with input from wildfire_msgs:msg/Detection.idl
// generated code does not contain a copyright notice
#include "wildfire_msgs/msg/detail/detection__rosidl_typesupport_fastrtps_cpp.hpp"
#include "wildfire_msgs/msg/detail/detection__struct.hpp"

#include <limits>
#include <stdexcept>
#include <string>
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_fastrtps_cpp/identifier.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_fastrtps_cpp/wstring_conversion.hpp"
#include "fastcdr/Cdr.h"


// forward declaration of message dependencies and their conversion functions

namespace wildfire_msgs
{

namespace msg
{

namespace typesupport_fastrtps_cpp
{

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_wildfire_msgs
cdr_serialize(
  const wildfire_msgs::msg::Detection & ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  // Member: found
  cdr << (ros_message.found ? true : false);
  // Member: cx
  cdr << ros_message.cx;
  // Member: cy
  cdr << ros_message.cy;
  // Member: area
  cdr << ros_message.area;
  // Member: img_w
  cdr << ros_message.img_w;
  // Member: img_h
  cdr << ros_message.img_h;
  // Member: confidence
  cdr << ros_message.confidence;
  // Member: use_confidence
  cdr << (ros_message.use_confidence ? true : false);
  return true;
}

bool
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_wildfire_msgs
cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  wildfire_msgs::msg::Detection & ros_message)
{
  // Member: found
  {
    uint8_t tmp;
    cdr >> tmp;
    ros_message.found = tmp ? true : false;
  }

  // Member: cx
  cdr >> ros_message.cx;

  // Member: cy
  cdr >> ros_message.cy;

  // Member: area
  cdr >> ros_message.area;

  // Member: img_w
  cdr >> ros_message.img_w;

  // Member: img_h
  cdr >> ros_message.img_h;

  // Member: confidence
  cdr >> ros_message.confidence;

  // Member: use_confidence
  {
    uint8_t tmp;
    cdr >> tmp;
    ros_message.use_confidence = tmp ? true : false;
  }

  return true;
}  // NOLINT(readability/fn_size)

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_wildfire_msgs
get_serialized_size(
  const wildfire_msgs::msg::Detection & ros_message,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // Member: found
  {
    size_t item_size = sizeof(ros_message.found);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: cx
  {
    size_t item_size = sizeof(ros_message.cx);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: cy
  {
    size_t item_size = sizeof(ros_message.cy);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: area
  {
    size_t item_size = sizeof(ros_message.area);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: img_w
  {
    size_t item_size = sizeof(ros_message.img_w);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: img_h
  {
    size_t item_size = sizeof(ros_message.img_h);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: confidence
  {
    size_t item_size = sizeof(ros_message.confidence);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // Member: use_confidence
  {
    size_t item_size = sizeof(ros_message.use_confidence);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

size_t
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_PUBLIC_wildfire_msgs
max_serialized_size_Detection(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;


  // Member: found
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }

  // Member: cx
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: cy
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: area
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: img_w
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: img_h
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: confidence
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  // Member: use_confidence
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint8_t);
    current_alignment += array_size * sizeof(uint8_t);
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = wildfire_msgs::msg::Detection;
    is_plain =
      (
      offsetof(DataType, use_confidence) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static bool _Detection__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  auto typed_message =
    static_cast<const wildfire_msgs::msg::Detection *>(
    untyped_ros_message);
  return cdr_serialize(*typed_message, cdr);
}

static bool _Detection__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  auto typed_message =
    static_cast<wildfire_msgs::msg::Detection *>(
    untyped_ros_message);
  return cdr_deserialize(cdr, *typed_message);
}

static uint32_t _Detection__get_serialized_size(
  const void * untyped_ros_message)
{
  auto typed_message =
    static_cast<const wildfire_msgs::msg::Detection *>(
    untyped_ros_message);
  return static_cast<uint32_t>(get_serialized_size(*typed_message, 0));
}

static size_t _Detection__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_Detection(full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}

static message_type_support_callbacks_t _Detection__callbacks = {
  "wildfire_msgs::msg",
  "Detection",
  _Detection__cdr_serialize,
  _Detection__cdr_deserialize,
  _Detection__get_serialized_size,
  _Detection__max_serialized_size
};

static rosidl_message_type_support_t _Detection__handle = {
  rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
  &_Detection__callbacks,
  get_message_typesupport_handle_function,
};

}  // namespace typesupport_fastrtps_cpp

}  // namespace msg

}  // namespace wildfire_msgs

namespace rosidl_typesupport_fastrtps_cpp
{

template<>
ROSIDL_TYPESUPPORT_FASTRTPS_CPP_EXPORT_wildfire_msgs
const rosidl_message_type_support_t *
get_message_type_support_handle<wildfire_msgs::msg::Detection>()
{
  return &wildfire_msgs::msg::typesupport_fastrtps_cpp::_Detection__handle;
}

}  // namespace rosidl_typesupport_fastrtps_cpp

#ifdef __cplusplus
extern "C"
{
#endif

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, wildfire_msgs, msg, Detection)() {
  return &wildfire_msgs::msg::typesupport_fastrtps_cpp::_Detection__handle;
}

#ifdef __cplusplus
}
#endif
