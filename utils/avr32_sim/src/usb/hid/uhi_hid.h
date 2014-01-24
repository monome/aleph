#ifndef _ALEPH_AVR32_USB_UHI_HID_H_
#define _ALEPH_AVR32_USB_UHI_HID_H_

#include "conf_usb_host.h"
#include "usb_protocol.h"
#include "usb_protocol_hid.h"
#include "uhi.h"

//! Global define which contains standard UHI API for UHC
//! It must be added in USB_HOST_UHI define from conf_usb_host.h file.
#define UHI_HID { \
	.install = uhi_hid_install, \
	.enable = uhi_hid_enable, \
	.uninstall = uhi_hid_uninstall, \
	.sof_notify = NULL, \
}
extern uhc_enum_status_t uhi_hid_install(uhc_device_t* dev);
extern void uhi_hid_enable(uhc_device_t* dev);
extern void uhi_hid_uninstall(uhc_device_t* dev);

#endif
