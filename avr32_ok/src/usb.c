/* usb.c
   aleph-avr32

   usb host implementation
 */

// asf
#include "compiler.h"
#include "print_funcs.h"
#include "uhd.h"
#include "uhi_hid_mouse.h"
#include "usb_protocol_hid.h"
// aleph
#include "types.h"
#include "usb.h"

// status of device enumeration
static uhc_enum_status_t enumStatus = UHC_ENUM_DISCONNECT;

// mouse state
static U8 mouseX;
static U8 mouseY;
static U8 mouseScroll;

// usb mode change callback
//void usb_mode_change(bool b_host_mode);
 
// usb Vbus change callback
void usb_vbus_change(bool b_vbus_present) {
  print_dbg("\r\n usb vbus change, new status: ");
  print_dbg_ulong(b_vbus_present);
}

// usb vbus error callback
void usb_vbus_error(void) {
  print_dbg("\r\n usb vbus error!");
}

// usb connection callback
void usb_connection(uhc_device_t *dev, bool b_present) {
    print_dbg("\r\n usb device connection: ");
    print_dbg_hex(dev);
    print_dbg(" , ");
    print_dbg_char(b_present);

    if(b_present) {
      ;;
    } else {
      enumStatus = UHC_ENUM_DISCONNECT;
    }

}

// usb wakeup callback
void usb_wakeup(void) {
    print_dbg("\r\n usb wakeup");
}

// usb start-of-frame callback
void usb_sof(void) {
  //// FIXME:
  /// don't want to do anything here, 
  //// in fact maybe this function call can  be disabled entirely in the UHC stack
  //    print_dbg("\r\n usb sof");
}

// usb end-of-enumeration callback
void usb_enum(uhc_device_t *dev, uhc_enum_status_t status) {
  print_dbg("\r\n usb enumerated: ");
  print_dbg_hex(dev);
  print_dbg(" , ");
  print_dbg_hex(status);

  enumStatus = status;
  if(status == UHC_ENUM_SUCCESS) {
    mouseX = 0;
    mouseY = 0;
    mouseScroll = 0;
  }
}

// mouse event callbacks
void hid_mouse_btn_left(bool b_state) {
  print_dbg("\r\n usb left mouse button: ");
  print_dbg_char(b_state);
}

void hid_mouse_btn_right(bool b_state) {
  print_dbg("\r\n usb right mouse button: ");
  print_dbg_char(b_state);
}

void hid_mouse_btn_middle(bool b_state) {
  print_dbg("\r\n usb mid mouse button: ");
  print_dbg_char(b_state);
}

void hid_mouse_move(int8_t x, int8_t y, int8_t scroll) {
  print_dbg("\r\n usb mouse move: ");
  print_dbg_ulong(x);
  print_dbg(", ");
  print_dbg_ulong(y);
}
