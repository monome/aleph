/*
  uhi_ftdi.h
  aleph-avr32

  usb host interface for FTDI driver

 */

#ifndef _UHI_FTDI_H_
#define _UHI_FTDI_H_

//#include "conf_usb_host.h"
//#include "usb_protocol.h"
//#include "uhi.h"

//! Global define which contains standard UHI API for UHC
//! It must be added in USB_HOST_UHI define from conf_usb_host.h file.
/* #define UHI_FTDI { \ */
/* 	.install = uhi_ftdi_install, \ */
/* 	.enable = uhi_ftdi_enable, \ */
/* 	.uninstall = uhi_ftdi_uninstall, \ */
/* 	.sof_notify = NULL, \ */
/* } */

#define FTDI_STRING_MAX_LEN 64

// install
extern uhc_enum_status_t uhi_ftdi_install(uhc_device_t* dev);
// uninstall
extern void uhi_ftdi_uninstall(uhc_device_t* dev);
// enable
extern void uhi_ftdi_enable(uhc_device_t* dev);
// input transfer
extern bool uhi_ftdi_in_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback);
// output transfer
extern bool uhi_ftdi_out_run(uint8_t * buf, iram_size_t buf_size,
		uhd_callback_trans_t callback);

// get string descriptions
extern void ftdi_get_strings(char** pManufacturer, char** pProduct, char** pSerial);

#endif // _UHI_FTDI_H_
