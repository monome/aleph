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
#include "types.h"

#include "midi.h"
#include "usb_protocol_midi.h"
#include "uhi_midi.h"

// define to see more stuff
#define UHI_MIDI_PRINT_DBG 1
// timeout in milliseconds
#define UHI_MIDI_TIMEOUT 5

// looks like we need to get class-specific endpoint descriptors,
//  to ask them about virtual cable index and stuff like that??

//...
#define MIDI_EP_DESC_REQ_TYPE ( (USB_REQ_DIR_IN) | (USB_REQ_TYPE_STANDARD) | (USB_REQ_RECIP_DEVICE) )


#define MIDI_EP_OUT_DESC_REQ
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

// read cable index

/* static void midi_get_cable(u8 descId, u8* cableIdx, u8* cableCount) {  */
/*   // send a setup request for the selected descriptor. */
/*   // descriptor type  */
/*   /\* */
/*     Table B-12: MIDI Adapter Class-specific Bulk IN Endpoint Descriptor */
/*     Offset */
/*     Field */
/*     Size Value */
/*     Description */

/*     0  bLength		 1 	0x05 	Size of this descriptor, in bytes. */
/*     1  bDescriptorType	 1 	0x25 	CS_ENDPOINT descriptor */
/*     2  bDescriptorSubtype 1 	0x01 	MS_GENERAL subtype. */
/*     3  bNumEmbMIDIJack 	 1 	0x01 	Number of embedded MIDI IN Jacks. */
/*     4  BaAssocJackID(1)	 1 	0x01 	ID of the Embedded MIDI IN Jack */

/*     // bulk OUT endpoint desc is similar with DescriptorType = 0x05 */
/*     *\/ */

/*   //    usb_setup_req_t req; */

/*   /\* req.bmRequestType = MIDI_EP_DESC_REQ_TYPE; *\/ */
/*   /\* req.bRequest = reqnum; *\/ */
/*   /\* req.wValue = (val); *\/ */
/*   /\* req.wIndex = (index); *\/ */
/*   /\* req.wLength = (size); *\/ */
/*   /\* return uhd_setup_request( *\/ */
/*   /\* 			   uhi_ftdi_dev.dev->address, *\/ */
/*   /\* 			   &req, *\/ */
/*   /\* 			   data, *\/ */
/*   /\* 			   size, *\/ */
/*   /\* 			   NULL, // no callback on run *\/ */
			   
/*   /\* 			   ); *\/ */
/* } */


//----- external (UHC) functions
uhc_enum_status_t uhi_midi_install(uhc_device_t* dev) {
  bool iface_supported;
  uint16_t conf_desc_lgt;
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
	  print_dbg(" input )");
	  uhi_midi_dev.ep_in = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;

	} else {
	  print_dbg(" output )");
	  uhi_midi_dev.ep_out = ((usb_ep_desc_t*)ptr_iface)->bEndpointAddress;
	}
	break;
      default:
	print_dbg("\r\n midi install weirdness: allocated endpoint, not recognizing type.");
	
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

  /* print_dbg("\r\n attempting to run midi input endpoint ; dev address: 0x"); */
  /* print_dbg_hex((u32) (uhi_midi_dev.dev->address) ); */
  /* print_dbg(" , endpoint number: "); */
  /* print_dbg_ulong((u32) (uhi_midi_dev.ep_in) ); */
 
  return uhd_ep_run(
		    uhi_midi_dev.dev->address,
		    uhi_midi_dev.ep_in, 
		    //		    false,  // shortpacket...
		    //// TEST:
		    true, 
		    buf, buf_size,
		    UHI_MIDI_TIMEOUT, 
		    callback);
}

bool uhi_midi_out_run(uint8_t * buf, iram_size_t buf_size,
		      uhd_callback_trans_t callback) {
  /*
    from uhd.h

    * \warning About \a b_shortpacket, for OUT endpoint it means that
    * a short packet or a Zero Length Packet must be sent to the USB line
    * to properly close the usb transfer at the end of the data transfer.
    * For Bulk and Interrupt IN endpoint, it will automatically stop the transfer
    * at the end of the data transfer (received short packet).
    *
   */

  /* print_dbg("\r\n attempting to run midi output endpoint ; dev address: 0x"); */
  /* print_dbg_hex((u32) (uhi_midi_dev.dev->address) ); */
  /* print_dbg(" , endpoint number: "); */
  /* print_dbg_ulong((u32) (uhi_midi_dev.ep_out) ); */
  
  return uhd_ep_run(
		    uhi_midi_dev.dev->address,
		    uhi_midi_dev.ep_out, 
		    true, // automatic shortpacket for buf < wlen
		    buf, buf_size,
		    UHI_MIDI_TIMEOUT, 
		    callback);
}
