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

#include "conf_usb_host.h"
#include "print_funcs.h"
#include "usb_protocol.h"
#include "uhd.h"
#include "uhc.h"
#include "uhi_hid_gamepad.h"
#include <string.h>

#ifdef USB_HOST_HUB_SUPPORT
# error USB HUB support is not implemented on UHI gamepad
#endif

// events
#define UHI_HID_GAMEPAD_BTN        0
#define UHI_HID_GAMEPAD_MOV_X      1
#define UHI_HID_GAMEPAD_MOV_Y      2
#define UHI_HID_GAMEPAD_MOV_SCROLL 3

// device data structure
typedef struct {
  uhc_device_t *dev;
  usb_ep_t ep_in;
  uint8_t report_size;
  uint8_t *report;
  //  uint8_t report_btn_prev;
}uhi_hid_gamepad_dev_t;

static uhi_hid_gamepad_dev_t uhi_hid_gamepad_dev = {
  .dev = NULL,
  .report = NULL,
};

//------- static funcs
static void uhi_hid_gamepad_start_trans_report(usb_add_t add);
static void uhi_hid_gamepad_report_reception(
					   usb_add_t add,
					   usb_ep_t ep,
					   uhd_trans_status_t status,
					   iram_size_t nb_transfered);
//@}

//----- external (UHC) functions
uhc_enum_status_t uhi_hid_gamepad_install(uhc_device_t* dev) {
  bool b_iface_supported;
  uint16_t conf_desc_lgt;
  usb_iface_desc_t *ptr_iface;

  if (uhi_hid_gamepad_dev.dev != NULL) {
    return UHC_ENUM_SOFTWARE_LIMIT; // Device already allocated
  }
  conf_desc_lgt = le16_to_cpu(dev->conf_desc->wTotalLength);
  ptr_iface = (usb_iface_desc_t*)dev->conf_desc;
  b_iface_supported = false;

  //  return UHC_ENUM_UNSUPPORTED; // No interface supported

  while(conf_desc_lgt) {
    switch (ptr_iface->bDescriptorType) {

    case USB_DT_INTERFACE:

      /////////////////
      ///////////////  
      //#if 1
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
      ////////////
      /////////
      if ((ptr_iface->bInterfaceClass   == HID_CLASS)
	  && (ptr_iface->bInterfaceProtocol == HID_PROTOCOL_GENERIC) ) {
	/////f
	//// FIXME:
	/// generic HID... assuming this is a gamepad for now!

	// Start allocation endpoint(s)
	b_iface_supported = true;
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
	return UHC_ENUM_HARDWARE_LIMIT; // Endpoint allocation fail
      }
      Assert(((usb_ep_desc_t*)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN);
      uhi_hid_gamepad_dev.ep_in = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
      uhi_hid_gamepad_dev.report_size =
	le16_to_cpu(((usb_ep_desc_t*)ptr_iface)->wMaxPacketSize);
      uhi_hid_gamepad_dev.report = malloc(uhi_hid_gamepad_dev.report_size);
      if (uhi_hid_gamepad_dev.report == NULL) {
	Assert(false);
	return UHC_ENUM_MEMORY_LIMIT; // Internal RAM allocation fail
      }
      uhi_hid_gamepad_dev.dev = dev;
      // All endpoints of all interfaces supported allocated
      return UHC_ENUM_SUCCESS;

    default:
      // Ignore descriptor
      break;
    }
    Assert(conf_desc_lgt>=ptr_iface->bLength);
    conf_desc_lgt -= ptr_iface->bLength;
    ptr_iface = (usb_iface_desc_t*)((uint8_t*)ptr_iface + ptr_iface->bLength);
  }
  return UHC_ENUM_UNSUPPORTED; // No interface supported
}
  /////////////////
  ////////////
// #endif ////
  //////////
  ////////////


void uhi_hid_gamepad_enable(uhc_device_t* dev) {
  if (uhi_hid_gamepad_dev.dev != dev) {
    return;  // No interface to enable
  }

  // Init value
  //uhi_hid_gamepad_dev.report_btn_prev = 0;
  uhi_hid_gamepad_start_trans_report(dev->address);
  UHI_HID_GAMEPAD_CHANGE(dev, true);
}

void uhi_hid_gamepad_uninstall(uhc_device_t* dev) {
  if (uhi_hid_gamepad_dev.dev != dev) {
    return; // Device not enabled in this interface
  }
  uhi_hid_gamepad_dev.dev = NULL;
  Assert(uhi_hid_gamepad_dev.report!=NULL);
  free(uhi_hid_gamepad_dev.report);
  UHI_HID_GAMEPAD_CHANGE(dev, false);
}

/**
 * \brief Starts the reception of the HID gamepad report
 *
 * \param add   USB address to use
 */
static void uhi_hid_gamepad_start_trans_report(usb_add_t add) {
  // Start transfer on interrupt endpoint IN
  uhd_ep_run(add, uhi_hid_gamepad_dev.ep_in, true, uhi_hid_gamepad_dev.report,
	     uhi_hid_gamepad_dev.report_size, 0, uhi_hid_gamepad_report_reception);
}

/**
 * \brief Decodes the HID gamepad report received
 *
 * \param add           USB address used by the transfer
 * \param status        Transfer status
 * \param nb_transfered Number of data transfered
 */
static void uhi_hid_gamepad_report_reception(
					   usb_add_t add,
					   usb_ep_t ep,
					   uhd_trans_status_t status,
					   iram_size_t nb_transfered)
{
  //  uint8_t state_prev;
  //uint8_t state_new;

  int i;

  UNUSED(ep);

  if ((status != UHD_TRANS_NOERROR) || (nb_transfered < 4)) {
    return; // HID gamepad transfer aborted
  }

  print_dbg("\r\n gamepad_report: ");
  for (i=0; i<uhi_hid_gamepad_dev.report_size; i++) {
    print_dbg(" ");
    print_dbg_hex((unsigned long int) uhi_hid_gamepad_dev.report[i]);
  }

  /* // Decode buttons */
  /* state_prev = uhi_hid_gamepad_dev.report_btn_prev; */
  /* state_new = uhi_hid_gamepad_dev.report[UHI_HID_GAMEPAD_BTN]; */
  /* if ((state_prev & 0x01) != (state_new & 0x01)) { */
  /*   //    UHI_HID_GAMEPAD_EVENT_BTN_LEFT((state_new & 0x01) ? true : false); */
  /* } */
  /* if ((state_prev & 0x02) != (state_new & 0x02)) { */
  /*   //    UHI_HID_GAMEPAD_EVENT_BTN_RIGHT((state_new & 0x02) ? true : false); */
  /* } */
  /* if ((state_prev & 0x04) != (state_new & 0x04)) { */
  /*   //    UHI_HID_GAMEPAD_EVENT_BTN_MIDDLE((state_new & 0x04) ? true : false); */
  /* } */
  /* uhi_hid_gamepad_dev.report_btn_prev = state_new; */

  // Decode moves
  /* if ((uhi_hid_gamepad_dev.report[UHI_HID_GAMEPAD_MOV_X] != 0) */
  /*     || (uhi_hid_gamepad_dev.report[UHI_HID_GAMEPAD_MOV_Y] != 0) */
  /*     || (uhi_hid_gamepad_dev.report[UHI_HID_GAMEPAD_MOV_SCROLL] != 0)) { */
  /*   UHI_HID_GAMEPAD_EVENT_MOUVE( */
  /* 			      (int8_t)uhi_hid_gamepad_dev.report[UHI_HID_GAMEPAD_MOV_X], */
  /* 			      (int8_t)uhi_hid_gamepad_dev.report[UHI_HID_GAMEPAD_MOV_Y], */
  /* 			      (int8_t)uhi_hid_gamepad_dev.report[UHI_HID_GAMEPAD_MOV_SCROLL]); */
  /* } */

  uhi_hid_gamepad_start_trans_report(add);
}
