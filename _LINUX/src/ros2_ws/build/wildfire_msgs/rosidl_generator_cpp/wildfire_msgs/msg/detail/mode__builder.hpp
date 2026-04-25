// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from wildfire_msgs:msg/Mode.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__MODE__BUILDER_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__MODE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "wildfire_msgs/msg/detail/mode__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace wildfire_msgs
{

namespace msg
{

namespace builder
{

class Init_Mode_stamp
{
public:
  explicit Init_Mode_stamp(::wildfire_msgs::msg::Mode & msg)
  : msg_(msg)
  {}
  ::wildfire_msgs::msg::Mode stamp(::wildfire_msgs::msg::Mode::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::wildfire_msgs::msg::Mode msg_;
};

class Init_Mode_mode
{
public:
  Init_Mode_mode()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Mode_stamp mode(::wildfire_msgs::msg::Mode::_mode_type arg)
  {
    msg_.mode = std::move(arg);
    return Init_Mode_stamp(msg_);
  }

private:
  ::wildfire_msgs::msg::Mode msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::wildfire_msgs::msg::Mode>()
{
  return wildfire_msgs::msg::builder::Init_Mode_mode();
}

}  // namespace wildfire_msgs

#endif  // WILDFIRE_MSGS__MSG__DETAIL__MODE__BUILDER_HPP_
