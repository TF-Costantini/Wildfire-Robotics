// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from wildfire_msgs:msg/DriveCmd.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__TRAITS_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "wildfire_msgs/msg/detail/drive_cmd__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace wildfire_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const DriveCmd & msg,
  std::ostream & out)
{
  out << "{";
  // member: left
  {
    out << "left: ";
    rosidl_generator_traits::value_to_yaml(msg.left, out);
    out << ", ";
  }

  // member: right
  {
    out << "right: ";
    rosidl_generator_traits::value_to_yaml(msg.right, out);
    out << ", ";
  }

  // member: stamp
  {
    out << "stamp: ";
    to_flow_style_yaml(msg.stamp, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const DriveCmd & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: left
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "left: ";
    rosidl_generator_traits::value_to_yaml(msg.left, out);
    out << "\n";
  }

  // member: right
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "right: ";
    rosidl_generator_traits::value_to_yaml(msg.right, out);
    out << "\n";
  }

  // member: stamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "stamp:\n";
    to_block_style_yaml(msg.stamp, out, indentation + 2);
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const DriveCmd & msg, bool use_flow_style = false)
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
  const wildfire_msgs::msg::DriveCmd & msg,
  std::ostream & out, size_t indentation = 0)
{
  wildfire_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use wildfire_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const wildfire_msgs::msg::DriveCmd & msg)
{
  return wildfire_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<wildfire_msgs::msg::DriveCmd>()
{
  return "wildfire_msgs::msg::DriveCmd";
}

template<>
inline const char * name<wildfire_msgs::msg::DriveCmd>()
{
  return "wildfire_msgs/msg/DriveCmd";
}

template<>
struct has_fixed_size<wildfire_msgs::msg::DriveCmd>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct has_bounded_size<wildfire_msgs::msg::DriveCmd>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct is_message<wildfire_msgs::msg::DriveCmd>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__TRAITS_HPP_
