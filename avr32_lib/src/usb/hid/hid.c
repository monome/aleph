/*
  hid.c
  aleph-avr32

  callbacks for HID gamepad
*/


#include "compiler.h"
#include "events.h"
#include "hid.h"

#define HID_FRAME_MAX_BYTES 8

static u8 frame[HID_FRAME_MAX_BYTES];
static event_t ev;

// parse frame and spawn events
void hid_parse_frame(u8* data, u8 size) {
  u8 i;

  // FIXME: one event per byte changed, pass byte idx and value as event data.
  // maybe this is not really too bad since most HID controls are only one byte.
  for(i=0; i<size; i++) {
    if(data[i] != frame[i]) {
      frame[i] = data[i];
      ev.type = kEventHidByte;
      ev.data = frame[i];
      ev.data |= (i << 8);
      event_post(&ev);
    }
  }
}

// get the frame data and size
const u8* hid_get_frame(void) {
  return (const u8*)frame;
}

const u8 hid_get_frame_size(void) {
  return HID_FRAME_MAX_BYTES;
}

#include "uhc.h"
