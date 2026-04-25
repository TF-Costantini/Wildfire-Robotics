#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to wildfire_msgs__msg__Mode
/// Mode.msg — Modalità operative del robot per Wildfire Robotics UGV

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct Mode {

    // This member is not documented.
    #[allow(missing_docs)]
    pub mode: u8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::Time,

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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Mode::default())
  }
}

impl rosidl_runtime_rs::Message for Mode {
  type RmwMsg = super::msg::rmw::Mode;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        mode: msg.mode,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.stamp)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      mode: msg.mode,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.stamp)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      mode: msg.mode,
      stamp: builtin_interfaces::msg::Time::from_rmw_message(msg.stamp),
    }
  }
}


// Corresponds to wildfire_msgs__msg__ButtonEvent
/// ButtonEvent.msg — Eventi del pulsante per Wildfire Robotics UGV

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct ButtonEvent {

    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::Time,


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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::ButtonEvent::default())
  }
}

impl rosidl_runtime_rs::Message for ButtonEvent {
  type RmwMsg = super::msg::rmw::ButtonEvent;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.stamp)).into_owned(),
        kind: msg.kind,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.stamp)).into_owned(),
      kind: msg.kind,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      stamp: builtin_interfaces::msg::Time::from_rmw_message(msg.stamp),
      kind: msg.kind,
    }
  }
}


// Corresponds to wildfire_msgs__msg__Detection
/// Detection.msg — Dati rilevamento (persona o fuoco) per Wildfire Robotics UGV

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::Detection::default())
  }
}

impl rosidl_runtime_rs::Message for Detection {
  type RmwMsg = super::msg::rmw::Detection;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        found: msg.found,
        cx: msg.cx,
        cy: msg.cy,
        area: msg.area,
        img_w: msg.img_w,
        img_h: msg.img_h,
        confidence: msg.confidence,
        use_confidence: msg.use_confidence,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      found: msg.found,
      cx: msg.cx,
      cy: msg.cy,
      area: msg.area,
      img_w: msg.img_w,
      img_h: msg.img_h,
      confidence: msg.confidence,
      use_confidence: msg.use_confidence,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      found: msg.found,
      cx: msg.cx,
      cy: msg.cy,
      area: msg.area,
      img_w: msg.img_w,
      img_h: msg.img_h,
      confidence: msg.confidence,
      use_confidence: msg.use_confidence,
    }
  }
}


// Corresponds to wildfire_msgs__msg__DriveCmd
/// DriveCmd.msg — Comando driving differenziale per Wildfire Robotics UGV
/// left/right: -1.0 (indietro max) .. 1.0 (avanti max)

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    pub stamp: builtin_interfaces::msg::Time,

}



impl Default for DriveCmd {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::DriveCmd::default())
  }
}

impl rosidl_runtime_rs::Message for DriveCmd {
  type RmwMsg = super::msg::rmw::DriveCmd;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        left: msg.left,
        right: msg.right,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.stamp)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      left: msg.left,
      right: msg.right,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.stamp)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      left: msg.left,
      right: msg.right,
      stamp: builtin_interfaces::msg::Time::from_rmw_message(msg.stamp),
    }
  }
}


// Corresponds to wildfire_msgs__msg__PanTiltCmd
/// PanTiltCmd.msg — Comando pan-tilt per Wildfire Robotics UGV
/// pan_deg: -60.0 .. +60.0, tilt_deg: 0.0 .. +30.0

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
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
    pub stamp: builtin_interfaces::msg::Time,

}



impl Default for PanTiltCmd {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::PanTiltCmd::default())
  }
}

impl rosidl_runtime_rs::Message for PanTiltCmd {
  type RmwMsg = super::msg::rmw::PanTiltCmd;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        pan_deg: msg.pan_deg,
        tilt_deg: msg.tilt_deg,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Owned(msg.stamp)).into_owned(),
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      pan_deg: msg.pan_deg,
      tilt_deg: msg.tilt_deg,
        stamp: builtin_interfaces::msg::Time::into_rmw_message(std::borrow::Cow::Borrowed(&msg.stamp)).into_owned(),
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      pan_deg: msg.pan_deg,
      tilt_deg: msg.tilt_deg,
      stamp: builtin_interfaces::msg::Time::from_rmw_message(msg.stamp),
    }
  }
}


