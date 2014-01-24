/* usb.c
   aleph-avr32

   usb host implementation
 */
//#include "aleph_board.h"
//#include "compiler.h"
#include "types.h"
//#include "conf_aleph.h"
//#include "gpio.h"
#include "print_funcs.h"
#include "usb.h"

/*
///// TODO! 
most of these things don't really need to be implemented
the ASF usb host stack needs them to be defined though.

we might be able to use vbus_error for overcurrent notifications.


   */

// usb mode change callback
void usb_mode_change(bool b_host_mode) {
  // print_dbg("\r\n mode change (ignore) ");
}
 
// usb Vbus change callback
void usb_vbus_change(bool b_vbus_present) {
  // print_dbg("\r\n usb vbus change, new status: ");
  // print_dbg_ulong(b_vbus_present);
}

// usb vbus error callback
void usb_vbus_error(void) {
  // print_dbg("\r\n usb vbus error: ");

}

// usb connection callback
/* void usb_connection(uhc_device_t *dev, bool b_present) { */
/*     // print_dbg("\r\n usb device connection: "); */
/*     // print_dbg_hex(dev); */
/*     // print_dbg(" , "); */
/*     // print_dbg_ulong(b_present); */
/* } */

// usb wakeup callback
void usb_wakeup(void) {
    // print_dbg("\r\n usb wakeup");
}

// usb start-of-frame callback
void usb_sof(void) {
  //    // print_dbg("\r\n usb sof");
}

/* // usb end-of-enumeration callback */
/* void usb_enum(uhc_device_t *dev, uhc_enum_status_t status) { */
/*   // print_dbg("\r\n usb enumerated: "); */
/*   // print_dbg_hex(dev); */
/*   // print_dbg(" , "); */
/*   // print_dbg_hex(status); */

/* } */
