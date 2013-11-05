/**
 * \file
 *
 * \brief USB host Vendor Interface driver.
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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

#include "conf_usb_host.h"
#include "usb_protocol.h"
#include "uhd.h"
#include "uhc.h"
#include "uhi_vendor.h"
#include <string.h>

#ifdef USB_HOST_HUB_SUPPORT
# error USB HUB support is not implemented on UHI vendor
#endif

#ifndef UHI_VENDOR_VID_PID_LIST
#error UHI_VENDOR_VID_PID_LIST must be defined in conf_usb_host.h like this {VID1,PID1},{VID2,PID2}...
#endif

/**
 * \ingroup uhi_vendor_group
 * \defgroup uhi_vendor_group_internal Implementation of UHI Vendor
 *
 * Class internal implementation
 * @{
 */

//! Timeout on Vendor transfer
#define  UHI_VENDOR_TIMEOUT 20000

/**
 * \name Structure to store information about USB Device Vendor
 */
//@{
typedef struct {
	uhc_device_t *dev;
	uint8_t bInterfaceNumber;
	usb_ep_t ep_bulk_in;
	usb_ep_t ep_bulk_out;
	usb_ep_t ep_int_in;
	usb_ep_t ep_int_out;
	usb_ep_t ep_iso_in;
	usb_ep_t ep_iso_out;
}uhi_vendor_dev_t;

static uhi_vendor_dev_t uhi_vendor_dev = {
	.dev = NULL
	};

//@}

/**
 * \name Functions required by UHC
 * @{
 */

uhc_enum_status_t uhi_vendor_install(uhc_device_t* dev)
{
	bool b_iface_supported;
	uint16_t conf_desc_lgt, vid, pid;
	usb_iface_desc_t *ptr_iface;
	struct {
		uint16_t vid;
		uint16_t pid;
	} vid_pid_supported[] = { UHI_VENDOR_VID_PID_LIST };

	if (uhi_vendor_dev.dev != NULL) {
		return UHC_ENUM_SOFTWARE_LIMIT; // Already allocated
	}

	// Check VID/PID (Mandatory for a Vendor Class)
	vid = le16_to_cpu(dev->dev_desc.idVendor);
	pid = le16_to_cpu(dev->dev_desc.idProduct);
	uint8_t i = 0;
	while(1) {
		if ((vid_pid_supported[i].vid == vid)
			&& (vid_pid_supported[i].pid == pid)) {
			// Valid USB device
			break;
		}
		if (++i == (sizeof(vid_pid_supported) /
				sizeof(vid_pid_supported[0]))) {
			// Unvalid USB device
			return UHC_ENUM_UNSUPPORTED;
		}
	}

	// Check that a vendor interface is present
	conf_desc_lgt = le16_to_cpu(dev->conf_desc->wTotalLength);
	ptr_iface = (usb_iface_desc_t*)dev->conf_desc;
	b_iface_supported = false;
	while(conf_desc_lgt) {
		switch (ptr_iface->bDescriptorType) {

		case USB_DT_INTERFACE:
			if ((ptr_iface->bInterfaceClass   == VENDOR_CLASS)
			&& (ptr_iface->bInterfaceSubClass == VENDOR_SUBCLASS)
			&& (ptr_iface->bInterfaceProtocol == VENDOR_PROTOCOL)
			&& (ptr_iface->bAlternateSetting == 1)) {
				// USB Vendor interface found
				// Start allocation endpoint(s)
				b_iface_supported = true;
				uhi_vendor_dev.bInterfaceNumber = ptr_iface->bInterfaceNumber;
				uhi_vendor_dev.ep_bulk_in = 0;
				uhi_vendor_dev.ep_bulk_out = 0;
				uhi_vendor_dev.ep_int_in = 0;
				uhi_vendor_dev.ep_int_out = 0;
				uhi_vendor_dev.ep_iso_in = 0;
				uhi_vendor_dev.ep_iso_out = 0;
			} else {
				if (b_iface_supported) {
					// End of Vendor Interface
					uhi_vendor_dev.dev = dev;
					return UHC_ENUM_SUCCESS;
				}
			}
			break;

		case USB_DT_ENDPOINT:
			//  Allocation of the endpoint
			if (!b_iface_supported) {
				break;
			}
			if (!uhd_ep_alloc(dev->address, (usb_ep_desc_t*)ptr_iface)) {
				return UHC_ENUM_HARDWARE_LIMIT; // Endpoint allocation fail
			}
			switch(((usb_ep_desc_t*)ptr_iface)->bmAttributes & USB_EP_TYPE_MASK) {
			case USB_EP_TYPE_INTERRUPT:
				if (((usb_ep_desc_t*)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN) {
					uhi_vendor_dev.ep_int_in = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
				} else {
					uhi_vendor_dev.ep_int_out = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
				}
				break;
			case USB_EP_TYPE_ISOCHRONOUS:
				if (((usb_ep_desc_t*)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN) {
					uhi_vendor_dev.ep_iso_in = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
				} else {
					uhi_vendor_dev.ep_iso_out = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
				}
				break;
			case USB_EP_TYPE_BULK:
				if (((usb_ep_desc_t*)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN) {
					uhi_vendor_dev.ep_bulk_in = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
				} else {
					uhi_vendor_dev.ep_bulk_out = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
				}
				break;
			default:
				return UHC_ENUM_SOFTWARE_LIMIT;
			}
			break;
		default:
			// Ignore descriptor
			break;
		}
		Assert(conf_desc_lgt>=ptr_iface->bLength);
		conf_desc_lgt -= ptr_iface->bLength;
		ptr_iface = (usb_iface_desc_t*)((uint8_t*)ptr_iface + ptr_iface->bLength);
	}
	if (b_iface_supported) {
		// End of Vendor Interface
		uhi_vendor_dev.dev = dev;
		return UHC_ENUM_SUCCESS;
	}
	return UHC_ENUM_UNSUPPORTED; // No interface supported
}

void uhi_vendor_enable(uhc_device_t* dev)
{
	usb_setup_req_t req;

	if (uhi_vendor_dev.dev != dev) {
		return;  // No interface to enable
	}

	// Choose the alternate setting 1 which contains all endpoints
	req.bmRequestType = USB_REQ_RECIP_INTERFACE;
	req.bRequest = USB_REQ_SET_INTERFACE;
	req.wValue = 1; // Alternate setting 1
	req.wIndex = uhi_vendor_dev.bInterfaceNumber;
	req.wLength = 0;
	uhd_setup_request(uhi_vendor_dev.dev->address,
			&req,
			NULL,
			0,
			NULL,
			NULL);

	UHI_VENDOR_CHANGE(dev, true);
}

void uhi_vendor_uninstall(uhc_device_t* dev)
{
	if (uhi_vendor_dev.dev != dev) {
		return; // Device not enabled in this interface
	}
	uhi_vendor_dev.dev = NULL;
	UHI_VENDOR_CHANGE(dev, false);
}
//@}

bool uhi_vendor_control_in_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_setup_end_t callback)
{
	usb_setup_req_t req;

	if (uhi_vendor_dev.dev == NULL) {
		return false;
	}
	req.bmRequestType = USB_REQ_RECIP_INTERFACE|USB_REQ_TYPE_VENDOR|USB_REQ_DIR_IN;
	req.bRequest = 0;
	req.wValue = 0;
	req.wIndex = 0;
	req.wLength = buf_size;
	return uhd_setup_request(uhi_vendor_dev.dev->address,
			&req,
			buf,
			buf_size,
			NULL,
			callback);
}


bool uhi_vendor_control_out_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_setup_end_t callback)
{
	usb_setup_req_t req;

	if (uhi_vendor_dev.dev == NULL) {
		return false;
	}
	req.bmRequestType = USB_REQ_RECIP_INTERFACE|USB_REQ_TYPE_VENDOR;
	req.bRequest = 0;
	req.wValue = 0;
	req.wIndex = 0;
	req.wLength = buf_size;
	return uhd_setup_request(uhi_vendor_dev.dev->address,
			&req,
			buf,
			buf_size,
			NULL,
			callback);
}

bool uhi_vendor_bulk_is_available(void)
{
	return (uhi_vendor_dev.dev != NULL) && uhi_vendor_dev.ep_bulk_in
			&& uhi_vendor_dev.ep_bulk_out;
}

bool uhi_vendor_bulk_in_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback)
{
	if (!uhi_vendor_bulk_is_available()) {
		return false;
	}
	return uhd_ep_run(uhi_vendor_dev.dev->address,
			uhi_vendor_dev.ep_bulk_in, false, buf, buf_size,
			UHI_VENDOR_TIMEOUT, callback);
}

bool uhi_vendor_bulk_out_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback)
{
	if (!uhi_vendor_bulk_is_available()) {
		return false;
	}
	return uhd_ep_run(uhi_vendor_dev.dev->address,
			uhi_vendor_dev.ep_bulk_out, false, buf, buf_size,
			UHI_VENDOR_TIMEOUT, callback);
}

bool uhi_vendor_int_is_available(void)
{
	return (uhi_vendor_dev.dev != NULL) && uhi_vendor_dev.ep_int_in
			&& uhi_vendor_dev.ep_int_out;
}

bool uhi_vendor_int_in_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback)
{
	if (!uhi_vendor_int_is_available()) {
		return false;
	}
	return uhd_ep_run(uhi_vendor_dev.dev->address,
			uhi_vendor_dev.ep_int_in, false, buf, buf_size,
			UHI_VENDOR_TIMEOUT, callback);
}

bool uhi_vendor_int_out_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback)
{
	if (!uhi_vendor_int_is_available()) {
		return false;
	}
	return uhd_ep_run(uhi_vendor_dev.dev->address,
			uhi_vendor_dev.ep_int_out, false, buf, buf_size,
			UHI_VENDOR_TIMEOUT, callback);
}

bool uhi_vendor_iso_is_available(void)
{
	return (uhi_vendor_dev.dev != NULL) && uhi_vendor_dev.ep_iso_in
			&& uhi_vendor_dev.ep_iso_out;
}

bool uhi_vendor_iso_in_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback)
{
	if (!uhi_vendor_iso_is_available()) {
		return false;
	}
	return uhd_ep_run(uhi_vendor_dev.dev->address,
			uhi_vendor_dev.ep_iso_in, false, buf, buf_size,
			UHI_VENDOR_TIMEOUT, callback);
}

bool uhi_vendor_iso_out_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback)
{
	if (!uhi_vendor_iso_is_available()) {
		return false;
	}
	return uhd_ep_run(uhi_vendor_dev.dev->address,
			uhi_vendor_dev.ep_iso_out, false, buf, buf_size,
			UHI_VENDOR_TIMEOUT, callback);
}

//@}
