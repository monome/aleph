#ifndef _ALEPH_HID_MOUSE_H_
#define _ALEPH_HID_MOUSE_H_

// mouse event callbacks
void hid_mouse_btn_left(bool b_state);
void hid_mouse_btn_right(bool b_state);
void hid_mouse_btn_middle(bool b_state);
void hid_mouse_move(int8_t x, int8_t y, int8_t scroll);

#endif // h guard
