/*
  uhi_midi.h
  aleph-avr32

  usb host interface for midi driver

 */

#ifndef _USB_UHI_MIDI_H_
#define _USB_UHI_MIDI_H_

//#include "conf_usb_host.h"
//#include "usb_protocol.h"
//#include "uhi.h"

//! Global define which contains standard UHI API for UHC
//! It must be added in USB_HOST_UHI define from conf_usb_host.h file.
/* #define UHI_MIDI { \ */
/* 	.install = uhi_midi_install, \ */
/* 	.enable = uhi_midi_enable, \ */
/* 	.uninstall = uhi_midi_uninstall, \ */
/* 	.sof_notify = NULL, \ */
/* } */


// install
	//extern uhc_enum_status_t uhi_midi_install(uhc_device_t* dev);
// uninstall
	//extern void uhi_midi_uninstall(uhc_device_t* dev);
// enable
	//extern void uhi_midi_enable(uhc_device_t* dev);
// input transfer
	//extern bool uhi_midi_in_run(uint8_t * buf, iram_size_t buf_size,
	//		uhd_callback_trans_t callback);
// output transfer
//extern bool uhi_midi_out_run(uint8_t * buf, iram_size_t buf_size,
	//		uhd_callback_trans_t callback);

// get string descriptions
//extern void midi_get_strings(char** pManufacturer, char** pProduct, char** pSerial);



#endif // h guard
