// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from wildfire_msgs:msg/Detection.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__DETECTION__TRAITS_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__DETECTION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "wildfire_msgs/msg/detail/detection__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace wildfire_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const Detection & msg,
  std::ostream & out)
{
  out << "{";
  // member: found
  {
    out << "found: ";
    rosidl_generator_traits::value_to_yaml(msg.found, out);
    out << ", ";
  }

  // member: cx
  {
    out << "cx: ";
    rosidl_generator_traits::value_to_yaml(msg.cx, out);
    out << ", ";
  }

  // member: cy
  {
    out << "cy: ";
    rosidl_generator_traits::value_to_yaml(msg.cy, out);
    out << ", ";
  }

  // member: area
  {
    out << "area: ";
    rosidl_generator_traits::value_to_yaml(msg.area, out);
    out << ", ";
  }

  // member: img_w
  {
    out << "img_w: ";
    rosidl_generator_traits::value_to_yaml(msg.img_w, out);
    out << ", ";
  }

  // member: img_h
  {
    out << "img_h: ";
    rosidl_generator_traits::value_to_yaml(msg.img_h, out);
    out << ", ";
  }

  // member: confidence
  {
    out << "confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.confidence, out);
    out << ", ";
  }

  // member: use_confidence
  {
    out << "use_confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.use_confidence, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const Detection & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: found
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "found: ";
    rosidl_generator_traits::value_to_yaml(msg.found, out);
    out << "\n";
  }

  // member: cx
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "cx: ";
    rosidl_generator_traits::value_to_yaml(msg.cx, out);
    out << "\n";
  }

  // member: cy
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "cy: ";
    rosidl_generator_traits::value_to_yaml(msg.cy, out);
    out << "\n";
  }

  // member: area
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "area: ";
    rosidl_generator_traits::value_to_yaml(msg.area, out);
    out << "\n";
  }

  // member: img_w
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "img_w: ";
    rosidl_generator_traits::value_to_yaml(msg.img_w, out);
    out << "\n";
  }

  // member: img_h
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "img_h: ";
    rosidl_generator_traits::value_to_yaml(msg.img_h, out);
    out << "\n";
  }

  // member: confidence
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.confidence, out);
    out << "\n";
  }

  // member: use_confidence
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "use_confidence: ";
    rosidl_generator_traits::value_to_yaml(msg.use_confidence, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const Detection & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace wildfire_msgs

namespace rosidl_generator_traits
{

[[deprecated("use wildfire_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const wildfire_msgs::msg::Detection & msg,
  std::ostream & out, size_t indentation = 0)
{
  wildfire_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use wildfire_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const wildfire_msgs::msg::Detection & msg)
{
  return wildfire_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<wildfire_msgs::msg::Detection>()
{
  return "wildfire_msgs::msg::Detection";
}

template<>
inline const char * name<wildfire_msgs::msg::Detection>()
{
  return "wildfire_msgs/msg/Detection";
}

template<>
struct has_fixed_size<wildfire_msgs::msg::Detection>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<wildfire_msgs::msg::Detection>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<wildfire_msgs::msg::Detection>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // WILDFIRE_MSGS__MSG__DETAIL__DETECTION__TRAITS_HPP_
