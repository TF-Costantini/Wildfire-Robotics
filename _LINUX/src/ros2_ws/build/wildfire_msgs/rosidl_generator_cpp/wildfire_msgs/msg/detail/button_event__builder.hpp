// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from wildfire_msgs:msg/ButtonEvent.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__BUILDER_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "wildfire_msgs/msg/detail/button_event__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace wildfire_msgs
{

namespace msg
{

namespace builder
{

class Init_ButtonEvent_kind
{
public:
  explicit Init_ButtonEvent_kind(::wildfire_msgs::msg::ButtonEvent & msg)
  : msg_(msg)
  {}
  ::wildfire_msgs::msg::ButtonEvent kind(::wildfire_msgs::msg::ButtonEvent::_kind_type arg)
  {
    msg_.kind = std::move(arg);
    return std::move(msg_);
  }

private:
  ::wildfire_msgs::msg::ButtonEvent msg_;
};

class Init_ButtonEvent_stamp
{
public:
  Init_ButtonEvent_stamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ButtonEvent_kind stamp(::wildfire_msgs::msg::ButtonEvent::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return Init_ButtonEvent_kind(msg_);
  }

private:
  ::wildfire_msgs::msg::ButtonEvent msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::wildfire_msgs::msg::ButtonEvent>()
{
  return wildfire_msgs::msg::builder::Init_ButtonEvent_stamp();
}

}  // namespace wildfire_msgs

#endif  // WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__BUILDER_HPP_
