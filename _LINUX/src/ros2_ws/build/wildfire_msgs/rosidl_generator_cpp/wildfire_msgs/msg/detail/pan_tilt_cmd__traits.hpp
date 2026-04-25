// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from wildfire_msgs:msg/PanTiltCmd.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__TRAITS_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "wildfire_msgs/msg/detail/pan_tilt_cmd__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace wildfire_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const PanTiltCmd & msg,
  std::ostream & out)
{
  out << "{";
  // member: pan_deg
  {
    out << "pan_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.pan_deg, out);
    out << ", ";
  }

  // member: tilt_deg
  {
    out << "tilt_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.tilt_deg, out);
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
  const PanTiltCmd & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: pan_deg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "pan_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.pan_deg, out);
    out << "\n";
  }

  // member: tilt_deg
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tilt_deg: ";
    rosidl_generator_traits::value_to_yaml(msg.tilt_deg, out);
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

inline std::string to_yaml(const PanTiltCmd & msg, bool use_flow_style = false)
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
  const wildfire_msgs::msg::PanTiltCmd & msg,
  std::ostream & out, size_t indentation = 0)
{
  wildfire_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use wildfire_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const wildfire_msgs::msg::PanTiltCmd & msg)
{
  return wildfire_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<wildfire_msgs::msg::PanTiltCmd>()
{
  return "wildfire_msgs::msg::PanTiltCmd";
}

template<>
inline const char * name<wildfire_msgs::msg::PanTiltCmd>()
{
  return "wildfire_msgs/msg/PanTiltCmd";
}

template<>
struct has_fixed_size<wildfire_msgs::msg::PanTiltCmd>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct has_bounded_size<wildfire_msgs::msg::PanTiltCmd>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct is_message<wildfire_msgs::msg::PanTiltCmd>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__TRAITS_HPP_
