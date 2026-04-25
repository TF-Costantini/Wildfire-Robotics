// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from wildfire_msgs:msg/Detection.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__DETECTION__STRUCT_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__DETECTION__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__wildfire_msgs__msg__Detection __attribute__((deprecated))
#else
# define DEPRECATED__wildfire_msgs__msg__Detection __declspec(deprecated)
#endif

namespace wildfire_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct Detection_
{
  using Type = Detection_<ContainerAllocator>;

  explicit Detection_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->found = false;
      this->cx = 0.0f;
      this->cy = 0.0f;
      this->area = 0.0f;
      this->img_w = 0.0f;
      this->img_h = 0.0f;
      this->confidence = 0.0f;
      this->use_confidence = false;
    }
  }

  explicit Detection_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->found = false;
      this->cx = 0.0f;
      this->cy = 0.0f;
      this->area = 0.0f;
      this->img_w = 0.0f;
      this->img_h = 0.0f;
      this->confidence = 0.0f;
      this->use_confidence = false;
    }
  }

  // field types and members
  using _found_type =
    bool;
  _found_type found;
  using _cx_type =
    float;
  _cx_type cx;
  using _cy_type =
    float;
  _cy_type cy;
  using _area_type =
    float;
  _area_type area;
  using _img_w_type =
    float;
  _img_w_type img_w;
  using _img_h_type =
    float;
  _img_h_type img_h;
  using _confidence_type =
    float;
  _confidence_type confidence;
  using _use_confidence_type =
    bool;
  _use_confidence_type use_confidence;

  // setters for named parameter idiom
  Type & set__found(
    const bool & _arg)
  {
    this->found = _arg;
    return *this;
  }
  Type & set__cx(
    const float & _arg)
  {
    this->cx = _arg;
    return *this;
  }
  Type & set__cy(
    const float & _arg)
  {
    this->cy = _arg;
    return *this;
  }
  Type & set__area(
    const float & _arg)
  {
    this->area = _arg;
    return *this;
  }
  Type & set__img_w(
    const float & _arg)
  {
    this->img_w = _arg;
    return *this;
  }
  Type & set__img_h(
    const float & _arg)
  {
    this->img_h = _arg;
    return *this;
  }
  Type & set__confidence(
    const float & _arg)
  {
    this->confidence = _arg;
    return *this;
  }
  Type & set__use_confidence(
    const bool & _arg)
  {
    this->use_confidence = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    wildfire_msgs::msg::Detection_<ContainerAllocator> *;
  using ConstRawPtr =
    const wildfire_msgs::msg::Detection_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      wildfire_msgs::msg::Detection_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      wildfire_msgs::msg::Detection_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__wildfire_msgs__msg__Detection
    std::shared_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__wildfire_msgs__msg__Detection
    std::shared_ptr<wildfire_msgs::msg::Detection_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Detection_ & other) const
  {
    if (this->found != other.found) {
      return false;
    }
    if (this->cx != other.cx) {
      return false;
    }
    if (this->cy != other.cy) {
      return false;
    }
    if (this->area != other.area) {
      return false;
    }
    if (this->img_w != other.img_w) {
      return false;
    }
    if (this->img_h != other.img_h) {
      return false;
    }
    if (this->confidence != other.confidence) {
      return false;
    }
    if (this->use_confidence != other.use_confidence) {
      return false;
    }
    return true;
  }
  bool operator!=(const Detection_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Detection_

// alias to use template instance with default allocator
using Detection =
  wildfire_msgs::msg::Detection_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace wildfire_msgs

#endif  // WILDFIRE_MSGS__MSG__DETAIL__DETECTION__STRUCT_HPP_
