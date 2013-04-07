/* usb.c
   aleph-avr32

   usb host implementation
 */

#include "compiler.h"
#include "conf_aleph.h"
#include "gpio.h"
#include "print_funcs.h"
#include "usb.h"

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
void usb_connection(uhc_device_t *dev, bool b_present) {
    // print_dbg("\r\n usb device connection: ");
    // print_dbg_hex(dev);
    // print_dbg(" , ");
    // print_dbg_ulong(b_present);
}

// usb wakeup callback
void usb_wakeup(void) {
    // print_dbg("\r\n usb wakeup");
}

// usb start-of-frame callback
void usb_sof(void) {
  //    // print_dbg("\r\n usb sof");
}

// usb end-of-enumeration callback
void usb_enum(uhc_device_t *dev, uhc_enum_status_t status) {
  // print_dbg("\r\n usb enumerated: ");
  // print_dbg_hex(dev);
  // print_dbg(" , ");
  // print_dbg_hex(status);

}

// mouse event callbacks
void hid_mouse_btn_left(bool b_state) {
  // print_dbg("\r\n usb left mouse button: ");
  // print_dbg_ulong(b_state);
  gpio_tgl_gpio_pin(LED_MODE_PIN);
}

void hid_mouse_btn_right(bool b_state) {
  // print_dbg("\r\n usb right mouse button: ");
  // print_dbg_ulong(b_state);
}

void hid_mouse_btn_middle(bool b_state) {
  // print_dbg("\r\n usb mid mouse button: ");
  // print_dbg_ulong(b_state);
}

void hid_mouse_move(int8_t x, int8_t y, int8_t scroll) {
  // print_dbg("\r\n usb mouse move; x: ");
  // print_dbg_hex(x);
  // print_dbg(", y: ");
  // print_dbg_hex(y);
  // print_dbg(", scroll: ");
  // print_dbg_hex(scroll);
}
