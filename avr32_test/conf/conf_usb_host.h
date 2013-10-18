#ifndef _CONF_USB_HOST_H_
#define _CONF_USB_HOST_H_

#include "compiler.h"

/**
 * USB Host Configuration
 * @{
 */

/// don't sleep
#define UHD_NO_SLEEP_MGR

//! Array of UHI APIs Define the list of UHI supported by USB host.
#define USB_HOST_UHI        UHI_HID_MOUSE , UHI_HID_GAMEPAD , UHI_FTDI , UHI_MIDI ,

//! Maximum current allowed on Vbus (mA)
#define USB_HOST_POWER_MAX  500

/**
 * USB Host callbacks
 * @{
 */

//! To notify that the USB mode are switched automatically.
//! This is possible only when ID pin is available.
#define UHC_MODE_CHANGE(b_host_mode)        usb_mode_change(b_host_mode)

//! To notify that the Vbus level has changed
//! Available only in USB hardware with Vbus monitoring.
#define UHC_VBUS_CHANGE(b_present)          usb_vbus_change(b_present)

//! To notify that a Vbus error has occurred
//! Available only in USB hardware with Vbus monitoring.
#define UHC_VBUS_ERROR()                    usb_vbus_error()

//! To notify that a device has been connected or disconnected.
#define UHC_CONNECTION_EVENT(dev,b_present) usb_connection(dev,b_present)

//! Called when a USB device or the host have wake up the USB line.
#define UHC_WAKEUP_EVENT()                  usb_wakeup()

//! Called for each received SOF each 1 ms
//! Note: Available in High and Full speed mode
#define UHC_SOF_EVENT()                     usb_sof()

//! Called when a USB device configuration must be chosen.
//! Thus, the application can choose either a configuration number for this device
//! or a configuration number 0 to reject it.
//! If callback not defined the configuration 1 is chosen.
//#define UHC_DEVICE_CONF(dev)                uint8_t usb_device_conf(dev)

//! Called when a USB device enumeration is completed.
#define UHC_ENUM_EVENT(dev,b_status)        usb_enum(dev,b_status)


// mouse functions
#define UHI_HID_MOUSE_CHANGE(dev,b_plug)
// FIXME : wtf why are these here
#define UHI_HID_MOUSE_EVENT_BTN_LEFT(b_state)    hid_mouse_btn_left(b_state)
#define UHI_HID_MOUSE_EVENT_BTN_RIGHT(b_state)   hid_mouse_btn_right(b_state)
#define UHI_HID_MOUSE_EVENT_BTN_MIDDLE(b_state)  hid_mouse_btn_middle(b_state)
#define UHI_HID_MOUSE_EVENT_MOUVE(x,y,scroll)    hid_mouse_move(x,y,scroll)

// gamepad functions
#define UHI_HID_GAMEPAD_CHANGE(dev,b_plug)

// ftdi functions
#define UHI_FTDI_CHANGE(dev, b_plug) ftdi_change(dev, b_plug)

// midi functions
#define UHI_MIDI_CHANGE(dev, b_plug) midi_change(dev, b_plug)


#include "uhi_ftdi.h"
#include "uhi_hid_mouse.h"
#include "uhi_hid_gamepad.h"
#include "uhi_midi.h"
//#include "ui.h"
#include "usb.h"

#endif // _CONF_USB_HOST_H_
