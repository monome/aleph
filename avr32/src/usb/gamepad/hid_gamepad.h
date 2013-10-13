#ifndef _ALEPH_HID_GAMEPAD_H_
#define _ALEPH_HID_GAMEPAD_H_


#include "compiler.h"
#include "uhc.h"

// parse frame and spawn events
void hid_gamepad_parse_frame(u8* data, u8 size);

// get the frame data and size
extern const u8* hid_gamepad_get_frame(void);
extern const u8 hid_gamepad_get_frame_size(void);

#endif // h guard
