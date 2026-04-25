// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from wildfire_msgs:msg/ButtonEvent.idl
// generated code does not contain a copyright notice

#ifndef WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__STRUCT_HPP_
#define WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__wildfire_msgs__msg__ButtonEvent __attribute__((deprecated))
#else
# define DEPRECATED__wildfire_msgs__msg__ButtonEvent __declspec(deprecated)
#endif

namespace wildfire_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ButtonEvent_
{
  using Type = ButtonEvent_<ContainerAllocator>;

  explicit ButtonEvent_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->kind = 0;
    }
  }

  explicit ButtonEvent_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->kind = 0;
    }
  }

  // field types and members
  using _stamp_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _stamp_type stamp;
  using _kind_type =
    uint8_t;
  _kind_type kind;

  // setters for named parameter idiom
  Type & set__stamp(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->stamp = _arg;
    return *this;
  }
  Type & set__kind(
    const uint8_t & _arg)
  {
    this->kind = _arg;
    return *this;
  }

  // constant declarations
  static constexpr uint8_t PRESS =
    0u;
  static constexpr uint8_t RELEASE =
    1u;
  static constexpr uint8_t LONG_PRESS =
    2u;

  // pointer types
  using RawPtr =
    wildfire_msgs::msg::ButtonEvent_<ContainerAllocator> *;
  using ConstRawPtr =
    const wildfire_msgs::msg::ButtonEvent_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      wildfire_msgs::msg::ButtonEvent_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      wildfire_msgs::msg::ButtonEvent_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__wildfire_msgs__msg__ButtonEvent
    std::shared_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__wildfire_msgs__msg__ButtonEvent
    std::shared_ptr<wildfire_msgs::msg::ButtonEvent_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ButtonEvent_ & other) const
  {
    if (this->stamp != other.stamp) {
      return false;
    }
    if (this->kind != other.kind) {
      return false;
    }
    return true;
  }
  bool operator!=(const ButtonEvent_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ButtonEvent_

// alias to use template instance with default allocator
using ButtonEvent =
  wildfire_msgs::msg::ButtonEvent_<std::allocator<void>>;

// constant definitions
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t ButtonEvent_<ContainerAllocator>::PRESS;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t ButtonEvent_<ContainerAllocator>::RELEASE;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t ButtonEvent_<ContainerAllocator>::LONG_PRESS;
#endif  // __cplusplus < 201703L

}  // namespace msg

}  // namespace wildfire_msgs

#endif  // WILDFIRE_MSGS__MSG__DETAIL__BUTTON_EVENT__STRUCT_HPP_
