// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from wildfire_msgs:msg/DriveCmd.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__BUILDER_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "wildfire_msgs/msg/detail/drive_cmd__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace wildfire_msgs
{

namespace msg
{

namespace builder
{

class Init_DriveCmd_stamp
{
public:
  explicit Init_DriveCmd_stamp(::wildfire_msgs::msg::DriveCmd & msg)
  : msg_(msg)
  {}
  ::wildfire_msgs::msg::DriveCmd stamp(::wildfire_msgs::msg::DriveCmd::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::wildfire_msgs::msg::DriveCmd msg_;
};

class Init_DriveCmd_right
{
public:
  explicit Init_DriveCmd_right(::wildfire_msgs::msg::DriveCmd & msg)
  : msg_(msg)
  {}
  Init_DriveCmd_stamp right(::wildfire_msgs::msg::DriveCmd::_right_type arg)
  {
    msg_.right = std::move(arg);
    return Init_DriveCmd_stamp(msg_);
  }

private:
  ::wildfire_msgs::msg::DriveCmd msg_;
};

class Init_DriveCmd_left
{
public:
  Init_DriveCmd_left()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_DriveCmd_right left(::wildfire_msgs::msg::DriveCmd::_left_type arg)
  {
    msg_.left = std::move(arg);
    return Init_DriveCmd_right(msg_);
  }

private:
  ::wildfire_msgs::msg::DriveCmd msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::wildfire_msgs::msg::DriveCmd>()
{
  return wildfire_msgs::msg::builder::Init_DriveCmd_left();
}

}  // namespace wildfire_msgs

#endif  // WILDFIRE_MSGS__MSG__DETAIL__DRIVE_CMD__BUILDER_HPP_
