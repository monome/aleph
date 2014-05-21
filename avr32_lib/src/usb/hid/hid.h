#ifndef _ALEPH_AVR32_USB_HID_H_
#define _ALEPH_AVR32_USB_HID_H_


#include "compiler.h"
#include "uhc.h"

// HID event data is is simply a bitfield indicating which bytes have changed in the frame.
// application handler can look up the relevant bytes in the frame data directly.

// test dirty flag for given byte in packet
extern u8 hid_get_byte_flag(u32 data, u8 byte);

// parse frame and spawn events
extern void hid_parse_frame(u8* data, u8 size);


// get the frame data and size
extern const u8* hid_get_frame_data(void);
extern const u8 hid_get_frame_size(void);

#endif // h guard
