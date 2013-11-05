/*
  uhi_midi.c
  aleph-avr32
  
  usb host interface for usb-midi
*/


// asf
#include <string.h>
//#include "delay.h"
#include "print_funcs.h"
#include "usb_protocol.h"
#include "uhd.h"
#include "uhc.h"
// aleph
#include "conf_usb_host.h"
// #include "global.h"
#include "midi.h"
#include "usb_protocol_midi.h"
#include "uhi_midi.h"

#define UHI_MIDI_PRINT_DBG 0

#define UHI_MIDI_TIMEOUT 20000

//----- data types
// device data structure
typedef struct {
  uhc_device_t *dev;
  usb_ep_t ep_in;
  usb_ep_t ep_out;
} uhi_midi_dev_t;

//----- static variables

// device data
static uhi_midi_dev_t uhi_midi_dev = {
  .dev = NULL,
  .ep_in = 0xf,
  .ep_out = 0xf,
};

//------- static funcs

// send control request
/* static u8 send_ctl_request(u8 reqtype, u8 reqnum,  */
/* 			   u8* data, u16 size, */
/* 			   u16 index, u16 val,  */
/* 			   uhd_callback_setup_end_t callbackEnd); */
// control request end
/* static void ctl_req_end( */
/* 			usb_add_t add, */
/* 			uhd_trans_status_t status, */
/* 			uint16_t payload_trans); */

//----- external (UHC) functions
uhc_enum_status_t uhi_midi_install(uhc_device_t* dev) {
  bool iface_supported;
  //bool is_audio;
  uint16_t conf_desc_lgt;
  //, vid, pid;
  usb_iface_desc_t *ptr_iface;
  

  if (uhi_midi_dev.dev != NULL) {
    return UHC_ENUM_SOFTWARE_LIMIT; // Device already allocated
  }

  conf_desc_lgt = le16_to_cpu(dev->conf_desc->wTotalLength);
  ptr_iface = (usb_iface_desc_t*)dev->conf_desc;
  iface_supported = false;

  while(conf_desc_lgt) {
    switch (ptr_iface->bDescriptorType) {

    case USB_DT_INTERFACE:

#if UHI_MIDI_PRINT_DBG
      print_dbg("\r\n bLength : 0x");
      print_dbg_hex( ptr_iface->bLength);
      print_dbg("\r\n bInterfaceNumber : 0x");
      print_dbg_hex( ptr_iface->bInterfaceNumber );
      print_dbg("\r\n bAlternateSetting : 0x");
      print_dbg_hex( ptr_iface->bAlternateSetting );
      print_dbg("\r\n bNumEndpoints : 0x");
      print_dbg_hex( ptr_iface->bNumEndpoints );
      print_dbg("\r\n bInterfaceClass : 0x");
      print_dbg_hex( ptr_iface->bInterfaceClass );
      print_dbg("\r\n bInterfaceSubClass : 0x");
      print_dbg_hex( ptr_iface->bInterfaceSubClass );
      print_dbg("\r\n bInterfaceProtocol : 0x");
      print_dbg_hex( ptr_iface->bInterfaceProtocol );
      print_dbg("\r\n iInterface : 0x");
      print_dbg_hex( ptr_iface->iInterface );

      print_dbg("\r\n\r\n\r\n");
#endif
      

      if ( ( ptr_iface->bInterfaceClass == USB_AUDIO_IFACE_CLASS ) 
	   && (ptr_iface->bInterfaceSubClass == USB_MIDI_IFACE_SUBCLASS) ) {

#if UHI_MIDI_PRINT_DBG
	print_dbg("\r\n class/subclass matches audio/MIDI. ");
#endif
	iface_supported = true;
	uhi_midi_dev.ep_in = 0;
	uhi_midi_dev.ep_out = 0;
      } else {
	//// we want to check for class-specific MS interface? (type: .... TODO)
#if UHI_MIDI_PRINT_DBG
	print_dbg("\r\n uhi_midi_install ignoring interface; class: 0x");
       	print_dbg_hex(ptr_iface->bInterfaceClass);
	print_dbg(" ; subclass: 0x");
	print_dbg_hex(ptr_iface->bInterfaceSubClass);
#endif
		      
	iface_supported = false;
      }
      break;

    case USB_DT_ENDPOINT:
      if (!iface_supported) {
	break;
      }
      if (!uhd_ep_alloc(dev->address, (usb_ep_desc_t*)ptr_iface)) {
	print_dbg("\r\n endpoint allocation failed");
	return UHC_ENUM_HARDWARE_LIMIT;
      }

      switch(((usb_ep_desc_t*)ptr_iface)->bmAttributes & USB_EP_TYPE_MASK) {
      case USB_EP_TYPE_BULK:
       	print_dbg("\r\n allocating bulk endpoint ( ");
	if (((usb_ep_desc_t*)ptr_iface)->bEndpointAddress & USB_EP_DIR_IN) {
	  /* print_dbg(" input )"); */
	  /* print_dbg("\r\n ( previous input : 0x"); */
	  /* print_dbg_hex((u32)uhi_midi_dev.ep_in); */
	  /* if(uhi_midi_dev.ep_in != 0) { print_dbg("\r\n reallocating midi input endpoint"); } */
	  uhi_midi_dev.ep_in = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
	} else {
	  print_dbg(" output )");
	  print_dbg("\r\n ( previous output : 0x");
	  print_dbg_hex((u32)uhi_midi_dev.ep_out);
	  if(uhi_midi_dev.ep_out != 0) { print_dbg("\r\n reallocating midi output endpoint"); }
	  uhi_midi_dev.ep_out = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
	}
	break;
      default:
	;; // ignore endpoint (shouldn't get here)
	break;
      }
      break;
    default:
      print_dbg("\r\n uhi_midi_install ignoring descriptor; type: 0x");
      print_dbg_hex(ptr_iface->bDescriptorType );
      break;
      
    }
      
    Assert(conf_desc_lgt>=ptr_iface->bLength);
    conf_desc_lgt -= ptr_iface->bLength;
    ptr_iface = (usb_iface_desc_t*)((uint8_t*)ptr_iface + ptr_iface->bLength);
  }

  if (iface_supported) {
    uhi_midi_dev.dev = dev;
    print_dbg("\r\n completed MIDI device install");
    return UHC_ENUM_SUCCESS;
  }
  return UHC_ENUM_UNSUPPORTED; // No interface supported
}

void uhi_midi_enable(uhc_device_t* dev) {
  //  usb_setup_req_t req;

  /* print_dbg("\r\n midi enable routine "); */

  if (uhi_midi_dev.dev != dev) {
    return;  // No interface to enable
  }

  /* send_ctl_request(MIDI_DEVICE_OUT_REQTYPE,  */
  /* 		   MIDI_REQ_BITMODE, */
  /* 		   NULL, 0, */
  ////  TODO: ??? any setup requests necessary?
  /* 		   1, 0xff,  */
  /* 		   NULL); */
  
  //  UHI_MIDI_CHANGE(dev, true);
  midi_change(dev, true);  
  print_dbg("\r\n finished uhi_midi_enable");
}

void uhi_midi_uninstall(uhc_device_t* dev) {
  if (uhi_midi_dev.dev != dev) {
    return; // Device not enabled in this interface
  }
  uhi_midi_dev.dev = NULL;
  Assert(uhi_midi_dev.report!=NULL);
  midi_change(dev, false);  
}

bool uhi_midi_in_run(uint8_t * buf, iram_size_t buf_size,
		     uhd_callback_trans_t callback) {
  return uhd_ep_run(uhi_midi_dev.dev->address,
		    uhi_midi_dev.ep_in, false, buf, buf_size,
		    UHI_MIDI_TIMEOUT, callback);
}

bool uhi_midi_out_run(uint8_t * buf, iram_size_t buf_size,
		      uhd_callback_trans_t callback) {
  return uhd_ep_run(uhi_midi_dev.dev->address,
		    uhi_midi_dev.ep_out, true, buf, buf_size,
		    UHI_MIDI_TIMEOUT, callback);
}
