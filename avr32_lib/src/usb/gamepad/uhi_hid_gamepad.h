#ifndef _UHI_HID_GAMEPAD_H_
#define _UHI_HID_GAMEPAD_H_

#include "conf_usb_host.h"
#include "usb_protocol.h"
#include "usb_protocol_hid.h"
#include "uhi.h"

//! Global define which contains standard UHI API for UHC
//! It must be added in USB_HOST_UHI define from conf_usb_host.h file.
#define UHI_HID_GAMEPAD { \
	.install = uhi_hid_gamepad_install, \
	.enable = uhi_hid_gamepad_enable, \
	.uninstall = uhi_hid_gamepad_uninstall, \
	.sof_notify = NULL, \
}
extern uhc_enum_status_t uhi_hid_gamepad_install(uhc_device_t* dev);
extern void uhi_hid_gamepad_enable(uhc_device_t* dev);
extern void uhi_hid_gamepad_uninstall(uhc_device_t* dev);

#endif // _UHI_HID_GAMEPAD_H_
