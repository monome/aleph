#ifndef _ALEPH_AVR32_USB_HID_H_
#define _ALEPH_AVR32_USB_HID_H_

#include "types.h"
//#include "uhc.h"

// parse frame and spawn events
void hid_parse_frame(u8* data, u8 size);

// get the frame data and size
extern const u8* hid_get_frame(void);
extern const u8 hid_get_frame_size(void);

#endif // h guard
