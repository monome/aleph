/**
 * \file
 *
 * \brief USB device configuration file
 *
 * Copyright (C) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"

/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_ASF_HIDMOUSE
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME          "HID Mouse"
// #define  USB_DEVICE_SERIAL_NAME           "12...EF"

/**
 * Device speeds support
 * @{
 */
//! To define a Low speed device
//#define  USB_DEVICE_LOW_SPEED

//! To authorize the High speed
#if (UC3A3||UC3A4)
//#define  USB_DEVICE_HS_SUPPORT
#elif (SAM3XA)
//#define  USB_DEVICE_HS_SUPPORT
#endif
//@}

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()               ui_device_sof_action()
#define  UDC_SUSPEND_EVENT()           ui_device_suspend_action()
#define  UDC_RESUME_EVENT()            ui_device_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
#define  UDC_REMOTEWAKEUP_ENABLE()     ui_device_remotewakeup_enable()
#define  UDC_REMOTEWAKEUP_DISABLE()    ui_device_remotewakeup_disable()
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of HID Mouse interface
 * @{
 */
//! Interface callback definition
#define  UDI_HID_MOUSE_ENABLE_EXT()    ui_device_mouse_enable()
#define  UDI_HID_MOUSE_DISABLE_EXT()   ui_device_mouse_disable()
//@}
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

#endif // _CONF_USB_H_

//! The includes of classes and other headers must be done
//! at the end of this file to avoid compile error.
#include "udi_hid_mouse_conf.h"
#include "ui.h"
