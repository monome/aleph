/*
  hid_mouse.c
  aleph-avr32

  callbacks for HID mouse
*/

#include "compiler.h"
#include "print_funcs.h"
#include "uhc.h"

#include "hid_mouse.h"

// mouse event callbacks
void hid_mouse_btn_left(bool b_state) {
  print_dbg("\r\n usb left mouse button: ");
  print_dbg_ulong(b_state);
  
  //gpio_tgl_gpio_pin(LED_MODE_PIN);
}

void hid_mouse_btn_right(bool b_state) {
  print_dbg("\r\n usb right mouse button: ");
  print_dbg_ulong(b_state);
}

void hid_mouse_btn_middle(bool b_state) {
  print_dbg("\r\n usb mid mouse button: ");
  print_dbg_ulong(b_state);
}

void hid_mouse_move(int8_t x, int8_t y, int8_t scroll) {
  print_dbg("\r\n usb mouse move; x: ");
  print_dbg_hex(x);
  print_dbg(", y: ");
  print_dbg_hex(y);
  print_dbg(", scroll: ");
  print_dbg_hex(scroll);
}
