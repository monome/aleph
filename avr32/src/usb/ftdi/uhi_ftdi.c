/**
 * \file
 *
 * \brief USB host Human Interface Device (HID) gamepad driver.
 *
 * Copyright (C) 2011 - 2012 Atmel Corporation. All rights reserved.
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

// asf
#include <string.h>
#include "print_funcs.h"
#include "usb_protocol.h"
#include "uhd.h"
#include "uhc.h"
// aleph
#include "conf_usb_host.h"
#include "global.h"
#include "usb_protocol_ftdi.h"
#include "uhi_ftdi.h"
//#include "udi_ftdi.h"

#ifdef USB_HOST_HUB_SUPPORT
# error USB HUB support is not implemented
#endif

#define UHI_FTDI_TIMEOUT 20000

// events
//#define UHI_HID_MOUSE_BTN        0
//#define UHI_HID_MOUSE_MOV_X      1
//#define UHI_HID_MOUSE_MOV_Y      2
//#define UHI_HID_MOUSE_MOV_SCROLL 3

// device data structure
typedef struct {
  uhc_device_t *dev;
  usb_ep_t ep_in;
  usb_ep_t ep_out;
} uhi_ftdi_dev_t;



static uhi_ftdi_dev_t uhi_ftdi_dev = {
  .dev = NULL,
};

//------- static funcs
//static void uhi_ftdi_start_trans_report(usb_add_t add);
/* static void uhi_ftdi_report_reception( */
/* 					   usb_add_t add, */
/* 					   usb_ep_t ep, */
/* 					   uhd_trans_status_t status, */
/* 					   iram_size_t nb_transfered); */
//@}

//----- external (UHC) functions
uhc_enum_status_t uhi_ftdi_install(uhc_device_t* dev) {
  bool b_iface_supported;
  uint16_t conf_desc_lgt, vid, pid;
  usb_iface_desc_t *ptr_iface;

  if (uhi_ftdi_dev.dev != NULL) {
    return UHC_ENUM_SOFTWARE_LIMIT; // Device already allocated
  }

  // check vid/pid
  vid = le16_to_cpu(dev->dev_desc.idVendor);
  pid = le16_to_cpu(dev->dev_desc.idProduct);

  if( (vid == FTDI_VID) && (pid == FTDI_PID) ) {
    ;; // this is an FTDI device, so continue
  } else {
    return UHC_ENUM_UNSUPPORTED;
  }

  conf_desc_lgt = le16_to_cpu(dev->conf_desc->wTotalLength);
  ptr_iface = (usb_iface_desc_t*)dev->conf_desc;
  b_iface_supported = false;


  print_dbg("\r\n\r\n");
  print_dbg("\r\n iface_desc -> bLength : ");
  print_dbg_hex(ptr_iface->bLength);
  print_dbg("\r\n iface_desc -> bDescriptorType : ");
  print_dbg_hex(ptr_iface->bDescriptorType);
  print_dbg("\r\n iface_desc -> bInterfaceNumber : ");
  print_dbg_hex(ptr_iface->bInterfaceNumber);
  print_dbg("\r\n iface_desc -> bAlternateSetting : ");
  print_dbg_hex(ptr_iface->bAlternateSetting);
  print_dbg("\r\n iface_desc -> bNumEndpoints : ");
  print_dbg_hex(ptr_iface->bNumEndpoints);
  print_dbg("\r\n iface_desc -> bInterfaceClass : ");
  print_dbg_hex(ptr_iface->bInterfaceClass);
  print_dbg("\r\n iface_desc -> bInterfaceSubClass : ");
  print_dbg_hex(ptr_iface->bInterfaceSubClass);
  print_dbg("\r\n iface_desc -> bInterfaceProtocol : ");
  print_dbg_hex(ptr_iface->bInterfaceProtocol);
  print_dbg("\r\n iface_desc -> iInterface : ");
  print_dbg_hex(ptr_iface->iInterface);
  print_dbg("\r\n\r\n");


  //  return UHC_ENUM_UNSUPPORTED; // No interface supported

  while(conf_desc_lgt) {
    switch (ptr_iface->bDescriptorType) {

    case USB_DT_INTERFACE:
      if ((ptr_iface->bInterfaceClass == FTDI_CLASS)
	  && (ptr_iface->bInterfaceProtocol == FTDI_PROTOCOL) ) {
	// Start allocation endpoint(s) 
	print_dbg("\r\n class/protocol matches FTDI. ");
	b_iface_supported = true;
	uhi_ftdi_dev.ep_in = 0;
	uhi_ftdi_dev.ep_out = 0;
      } else {
	// Stop allocation endpoint(s)
	b_iface_supported = false;
      }
      break;

    case USB_DT_ENDPOINT:
      //  Allocation of the endpoint
      if (!b_iface_supported) {
	break;
      }
      if (!uhd_ep_alloc(dev->address, (usb_ep_desc_t*)ptr_iface)) {
	print_dbg("\r\n endpoint allocation failed");
	return UHC_ENUM_HARDWARE_LIMIT; // Endpoint allocation fail
      }

      switch(((usb_ep_desc_t*)ptr_iface)->bmAttributes & USB_EP_TYPE_MASK) {

      case USB_EP_TYPE_BULK:
	print_dbg("\r\n allocating bulk endpoint: ");
	if (((usb_ep_desc_t*)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN) {
	  print_dbg(" input");
	  uhi_ftdi_dev.ep_in = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
	} else {
	  print_dbg(" output");
	  uhi_ftdi_dev.ep_out = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
	}
	break;
      default:
	;; // ignore endpoint (shouldn't get here)
	break;
      }
      break;

    case	USB_DT_DEVICE :
      print_dbg("\r\n unhandled device descriptor");
      break;
    case	USB_DT_CONFIGURATION :
      print_dbg("\r\n unhandled config descriptor");
      break;
    case	USB_DT_STRING :
      print_dbg("\r\n unhandled string descriptor");
      break;
    case	USB_DT_DEVICE_QUALIFIER :
      print_dbg("\r\n unhandled device qualifier descriptor");
      break;
    case	USB_DT_OTHER_SPEED_CONFIGURATION :
      print_dbg("\r\n unhandled speed config descriptor");
      break;
    case	USB_DT_INTERFACE_POWER :
      print_dbg("\r\n unhandled interface power descriptor");
      break;
    case	USB_DT_OTG :
      print_dbg("\r\n unhandled OTG descriptor");
      break;
    case	USB_DT_IAD :
      print_dbg("\r\n unhandled IAD descriptor");
      break;
      
    default:
      //      ;; // ignore descriptor (shouldn't get here??)
      
      break;
    }
    Assert(conf_desc_lgt>=ptr_iface->bLength);
    conf_desc_lgt -= ptr_iface->bLength;
    ptr_iface = (usb_iface_desc_t*)((uint8_t*)ptr_iface + ptr_iface->bLength);
  }

  if (b_iface_supported) {
    uhi_ftdi_dev.dev = dev;
    print_dbg("\r\n completed FTDI device install");
    return UHC_ENUM_SUCCESS;
  }
  return UHC_ENUM_UNSUPPORTED; // No interface supported
}

void uhi_ftdi_enable(uhc_device_t* dev) {
  usb_setup_req_t req;

  if (uhi_ftdi_dev.dev != dev) {
    return;  // No interface to enable
  }

  ftdiPlug = 1;

  /* print_dbg("\r\n manufacturer string: "); */
  /* print_dbg(uhc_dev_get_string_manufacturer(uhi_ftdi_dev.dev)); */
  /* print_dbg("\r\n product string: "); */
  /* print_dbg(uhc_dev_get_string_product(uhi_ftdi_dev.dev)); */
  /* print_dbg("\r\n serial string: "); */
  /* print_dbg(uhc_dev_get_string_serial(uhi_ftdi_dev.dev)); */
  
  ////// testing
  ////////
    return;
  ///////////
  /////////////

  print_dbg("\r\n installed FTDI device, sending setup ");


  // Choose the alternate setting 1 which contains all endpoints
  req.bmRequestType = USB_REQ_RECIP_INTERFACE;
  req.bRequest = USB_REQ_SET_INTERFACE;
  req.wValue = 1; // Alternate setting 1
  req.wIndex = 0;
  req.wLength = 0;
  uhd_setup_request(uhi_ftdi_dev.dev->address,
		    &req,
		    NULL,
		    0,
		    NULL,
		    NULL);		    
  
  //  UHI_FTDI_CHANGE(dev, true);
}

void uhi_ftdi_uninstall(uhc_device_t* dev) {
  if (uhi_ftdi_dev.dev != dev) {
    return; // Device not enabled in this interface
  }
  uhi_ftdi_dev.dev = NULL;
  Assert(uhi_ftdi_dev.report!=NULL);
  ftdiPlug = 0;

  //  free(uhi_ftdi_dev.report);
  //UHI_FTDI_CHANGE(dev, false);
}


bool uhi_ftdi_in_run(uint8_t * buf, iram_size_t buf_size,
		     uhd_callback_trans_t callback) {
  return uhd_ep_run(uhi_ftdi_dev.dev->address,
		    uhi_ftdi_dev.ep_in, false, buf, buf_size,
		    UHI_FTDI_TIMEOUT, callback);
}

bool uhi_ftdi_out_run(uint8_t * buf, iram_size_t buf_size,
		      uhd_callback_trans_t callback) {
  return uhd_ep_run(uhi_ftdi_dev.dev->address,
		    uhi_ftdi_dev.ep_out, false, buf, buf_size,
		    UHI_FTDI_TIMEOUT, callback);
}
