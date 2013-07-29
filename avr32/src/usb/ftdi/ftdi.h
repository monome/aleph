#ifndef _ALEPH_FTD_H_
#define _ALEPH_FTDI_H_


#include "types.h"
#include "uhc.h"

#define FTDI_IN_BUF_SIZE 64
#define FTDI_OUT_BUF_SIZE 64

extern void ftdi_send(void);
extern void ftdi_receive(void);
extern void ftdi_change(uhc_device_t* dev, u8 plug);
/* // mouse event callbacks */
/* void hid_mouse_btn_left(bool b_state); */
/* void hid_mouse_btn_right(bool b_state); */
/* void hid_mouse_btn_middle(bool b_state); */
/* void hid_mouse_move(int8_t x, int8_t y, int8_t scroll); */



#endif // h guard
