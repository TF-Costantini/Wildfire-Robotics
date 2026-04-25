// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from wildfire_msgs:msg/PanTiltCmd.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__BUILDER_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "wildfire_msgs/msg/detail/pan_tilt_cmd__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace wildfire_msgs
{

namespace msg
{

namespace builder
{

class Init_PanTiltCmd_stamp
{
public:
  explicit Init_PanTiltCmd_stamp(::wildfire_msgs::msg::PanTiltCmd & msg)
  : msg_(msg)
  {}
  ::wildfire_msgs::msg::PanTiltCmd stamp(::wildfire_msgs::msg::PanTiltCmd::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::wildfire_msgs::msg::PanTiltCmd msg_;
};

class Init_PanTiltCmd_tilt_deg
{
public:
  explicit Init_PanTiltCmd_tilt_deg(::wildfire_msgs::msg::PanTiltCmd & msg)
  : msg_(msg)
  {}
  Init_PanTiltCmd_stamp tilt_deg(::wildfire_msgs::msg::PanTiltCmd::_tilt_deg_type arg)
  {
    msg_.tilt_deg = std::move(arg);
    return Init_PanTiltCmd_stamp(msg_);
  }

private:
  ::wildfire_msgs::msg::PanTiltCmd msg_;
};

class Init_PanTiltCmd_pan_deg
{
public:
  Init_PanTiltCmd_pan_deg()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_PanTiltCmd_tilt_deg pan_deg(::wildfire_msgs::msg::PanTiltCmd::_pan_deg_type arg)
  {
    msg_.pan_deg = std::move(arg);
    return Init_PanTiltCmd_tilt_deg(msg_);
  }

private:
  ::wildfire_msgs::msg::PanTiltCmd msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::wildfire_msgs::msg::PanTiltCmd>()
{
  return wildfire_msgs::msg::builder::Init_PanTiltCmd_pan_deg();
}

}  // namespace wildfire_msgs

#endif  // WILDFIRE_MSGS__MSG__DETAIL__PAN_TILT_CMD__BUILDER_HPP_
