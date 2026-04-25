// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from wildfire_msgs:msg/Detection.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__DETECTION__BUILDER_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__DETECTION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "wildfire_msgs/msg/detail/detection__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace wildfire_msgs
{

namespace msg
{

namespace builder
{

class Init_Detection_use_confidence
{
public:
  explicit Init_Detection_use_confidence(::wildfire_msgs::msg::Detection & msg)
  : msg_(msg)
  {}
  ::wildfire_msgs::msg::Detection use_confidence(::wildfire_msgs::msg::Detection::_use_confidence_type arg)
  {
    msg_.use_confidence = std::move(arg);
    return std::move(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

class Init_Detection_confidence
{
public:
  explicit Init_Detection_confidence(::wildfire_msgs::msg::Detection & msg)
  : msg_(msg)
  {}
  Init_Detection_use_confidence confidence(::wildfire_msgs::msg::Detection::_confidence_type arg)
  {
    msg_.confidence = std::move(arg);
    return Init_Detection_use_confidence(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

class Init_Detection_img_h
{
public:
  explicit Init_Detection_img_h(::wildfire_msgs::msg::Detection & msg)
  : msg_(msg)
  {}
  Init_Detection_confidence img_h(::wildfire_msgs::msg::Detection::_img_h_type arg)
  {
    msg_.img_h = std::move(arg);
    return Init_Detection_confidence(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

class Init_Detection_img_w
{
public:
  explicit Init_Detection_img_w(::wildfire_msgs::msg::Detection & msg)
  : msg_(msg)
  {}
  Init_Detection_img_h img_w(::wildfire_msgs::msg::Detection::_img_w_type arg)
  {
    msg_.img_w = std::move(arg);
    return Init_Detection_img_h(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

class Init_Detection_area
{
public:
  explicit Init_Detection_area(::wildfire_msgs::msg::Detection & msg)
  : msg_(msg)
  {}
  Init_Detection_img_w area(::wildfire_msgs::msg::Detection::_area_type arg)
  {
    msg_.area = std::move(arg);
    return Init_Detection_img_w(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

class Init_Detection_cy
{
public:
  explicit Init_Detection_cy(::wildfire_msgs::msg::Detection & msg)
  : msg_(msg)
  {}
  Init_Detection_area cy(::wildfire_msgs::msg::Detection::_cy_type arg)
  {
    msg_.cy = std::move(arg);
    return Init_Detection_area(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

class Init_Detection_cx
{
public:
  explicit Init_Detection_cx(::wildfire_msgs::msg::Detection & msg)
  : msg_(msg)
  {}
  Init_Detection_cy cx(::wildfire_msgs::msg::Detection::_cx_type arg)
  {
    msg_.cx = std::move(arg);
    return Init_Detection_cy(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

class Init_Detection_found
{
public:
  Init_Detection_found()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Detection_cx found(::wildfire_msgs::msg::Detection::_found_type arg)
  {
    msg_.found = std::move(arg);
    return Init_Detection_cx(msg_);
  }

private:
  ::wildfire_msgs::msg::Detection msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::wildfire_msgs::msg::Detection>()
{
  return wildfire_msgs::msg::builder::Init_Detection_found();
}

}  // namespace wildfire_msgs

#endif  // WILDFIRE_MSGS__MSG__DETAIL__DETECTION__BUILDER_HPP_
