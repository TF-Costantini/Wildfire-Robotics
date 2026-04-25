#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "wildfire_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__Mode() -> *const std::ffi::c_void;
}

#[link(name = "wildfire_msgs__rosidl_generator_c")]
extern "C" {
    fn wildfire_msgs__msg__Mode__init(msg: *mut Mode) -> bool;
    fn wildfire_msgs__msg__Mode__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Mode>, size: usize) -> bool;
    fn wildfire_msgs__msg__Mode__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Mode>);
    fn wildfire_msgs__msg__Mode__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Mode>, out_seq: *mut rosidl_runtime_rs::Sequence<Mode>) -> bool;
}

// Corresponds to wildfire_msgs__msg__Mode
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Mode.msg — Modalità operative del robot per Wildfire Robotics UGV

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Mode {

    // This member is not documented.
    #[allow(missing_docs)]
    pub mode: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::rmw::Time,

}

impl Mode {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const IDLE: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const FOLLOW: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const FIRE: u8 = 2;

}


impl Default for Mode {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !wildfire_msgs__msg__Mode__init(&mut msg as *mut _) {
        panic!("Call to wildfire_msgs__msg__Mode__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Mode {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__Mode__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__Mode__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__Mode__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Mode {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Mode where Self: Sized {
  const TYPE_NAME: &'static str = "wildfire_msgs/msg/Mode";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__Mode() }
  }
}


#[link(name = "wildfire_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__ButtonEvent() -> *const std::ffi::c_void;
}

#[link(name = "wildfire_msgs__rosidl_generator_c")]
extern "C" {
    fn wildfire_msgs__msg__ButtonEvent__init(msg: *mut ButtonEvent) -> bool;
    fn wildfire_msgs__msg__ButtonEvent__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<ButtonEvent>, size: usize) -> bool;
    fn wildfire_msgs__msg__ButtonEvent__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<ButtonEvent>);
    fn wildfire_msgs__msg__ButtonEvent__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<ButtonEvent>, out_seq: *mut rosidl_runtime_rs::Sequence<ButtonEvent>) -> bool;
}

// Corresponds to wildfire_msgs__msg__ButtonEvent
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// ButtonEvent.msg — Eventi del pulsante per Wildfire Robotics UGV

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ButtonEvent {

    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::rmw::Time,


    // This member is not documented.
    #[allow(missing_docs)]
    pub kind: u8,

}

impl ButtonEvent {

    // This constant is not documented.
    #[allow(missing_docs)]
    pub const PRESS: u8 = 0;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const RELEASE: u8 = 1;


    // This constant is not documented.
    #[allow(missing_docs)]
    pub const LONG_PRESS: u8 = 2;

}


impl Default for ButtonEvent {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !wildfire_msgs__msg__ButtonEvent__init(&mut msg as *mut _) {
        panic!("Call to wildfire_msgs__msg__ButtonEvent__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for ButtonEvent {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__ButtonEvent__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__ButtonEvent__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__ButtonEvent__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for ButtonEvent {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for ButtonEvent where Self: Sized {
  const TYPE_NAME: &'static str = "wildfire_msgs/msg/ButtonEvent";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__ButtonEvent() }
  }
}


#[link(name = "wildfire_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__Detection() -> *const std::ffi::c_void;
}

#[link(name = "wildfire_msgs__rosidl_generator_c")]
extern "C" {
    fn wildfire_msgs__msg__Detection__init(msg: *mut Detection) -> bool;
    fn wildfire_msgs__msg__Detection__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<Detection>, size: usize) -> bool;
    fn wildfire_msgs__msg__Detection__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<Detection>);
    fn wildfire_msgs__msg__Detection__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<Detection>, out_seq: *mut rosidl_runtime_rs::Sequence<Detection>) -> bool;
}

// Corresponds to wildfire_msgs__msg__Detection
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// Detection.msg — Dati rilevamento (persona o fuoco) per Wildfire Robotics UGV

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Detection {

    // This member is not documented.
    #[allow(missing_docs)]
    pub found: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub cx: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub cy: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub area: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub img_w: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub img_h: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub confidence: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub use_confidence: bool,

}



impl Default for Detection {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !wildfire_msgs__msg__Detection__init(&mut msg as *mut _) {
        panic!("Call to wildfire_msgs__msg__Detection__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for Detection {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__Detection__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__Detection__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__Detection__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for Detection {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for Detection where Self: Sized {
  const TYPE_NAME: &'static str = "wildfire_msgs/msg/Detection";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__Detection() }
  }
}


#[link(name = "wildfire_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__DriveCmd() -> *const std::ffi::c_void;
}

#[link(name = "wildfire_msgs__rosidl_generator_c")]
extern "C" {
    fn wildfire_msgs__msg__DriveCmd__init(msg: *mut DriveCmd) -> bool;
    fn wildfire_msgs__msg__DriveCmd__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<DriveCmd>, size: usize) -> bool;
    fn wildfire_msgs__msg__DriveCmd__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<DriveCmd>);
    fn wildfire_msgs__msg__DriveCmd__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<DriveCmd>, out_seq: *mut rosidl_runtime_rs::Sequence<DriveCmd>) -> bool;
}

// Corresponds to wildfire_msgs__msg__DriveCmd
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// DriveCmd.msg — Comando driving differenziale per Wildfire Robotics UGV
/// left/right: -1.0 (indietro max) .. 1.0 (avanti max)

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DriveCmd {

    // This member is not documented.
    #[allow(missing_docs)]
    pub left: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub right: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::rmw::Time,

}



impl Default for DriveCmd {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !wildfire_msgs__msg__DriveCmd__init(&mut msg as *mut _) {
        panic!("Call to wildfire_msgs__msg__DriveCmd__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for DriveCmd {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__DriveCmd__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__DriveCmd__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__DriveCmd__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for DriveCmd {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for DriveCmd where Self: Sized {
  const TYPE_NAME: &'static str = "wildfire_msgs/msg/DriveCmd";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__DriveCmd() }
  }
}


#[link(name = "wildfire_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__PanTiltCmd() -> *const std::ffi::c_void;
}

#[link(name = "wildfire_msgs__rosidl_generator_c")]
extern "C" {
    fn wildfire_msgs__msg__PanTiltCmd__init(msg: *mut PanTiltCmd) -> bool;
    fn wildfire_msgs__msg__PanTiltCmd__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<PanTiltCmd>, size: usize) -> bool;
    fn wildfire_msgs__msg__PanTiltCmd__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<PanTiltCmd>);
    fn wildfire_msgs__msg__PanTiltCmd__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<PanTiltCmd>, out_seq: *mut rosidl_runtime_rs::Sequence<PanTiltCmd>) -> bool;
}

// Corresponds to wildfire_msgs__msg__PanTiltCmd
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]

/// PanTiltCmd.msg — Comando pan-tilt per Wildfire Robotics UGV
/// pan_deg: -60.0 .. +60.0, tilt_deg: 0.0 .. +30.0

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct PanTiltCmd {

    // This member is not documented.
    #[allow(missing_docs)]
    pub pan_deg: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub tilt_deg: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::rmw::Time,

}



impl Default for PanTiltCmd {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !wildfire_msgs__msg__PanTiltCmd__init(&mut msg as *mut _) {
        panic!("Call to wildfire_msgs__msg__PanTiltCmd__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for PanTiltCmd {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__PanTiltCmd__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__PanTiltCmd__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { wildfire_msgs__msg__PanTiltCmd__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for PanTiltCmd {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for PanTiltCmd where Self: Sized {
  const TYPE_NAME: &'static str = "wildfire_msgs/msg/PanTiltCmd";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__wildfire_msgs__msg__PanTiltCmd() }
  }
}


