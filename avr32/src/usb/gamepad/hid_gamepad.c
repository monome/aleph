/*
  hid_gamepad.c
  aleph-avr32

  callbacks for HID gamepad
*/


#include "compiler.h"
#include "events.h"
#include "hid_gamepad.h"

#define HID_FRAME_MAX_BYTES 8

static u8 frame[HID_FRAME_MAX_BYTES];
static event_t ev;

// parse frame and spawn events
void hid_gamepad_parse_frame(u8* data, u8 size) {
  u8 i;


  // FIXME: one event per byte changed, pass byte idx and value as event data.
  // maybe this is not really too bad since most HID controls are only one byte.
  for(i=0; i<size; i++) {
    if(data[i] != frame[i]) {
      frame[i] = data[i];
      ev.eventType = kEventHidByte;
      ev.eventData = frame[i];
      ev.eventData |= (i << 8);
      post_event(&ev);
    }
  }
}

// get the frame data and size
const u8* hid_gamepad_get_frame(void) {
  return (const u8*)frame;
}

const u8 hid_gamepad_get_frame_size(void) {
  return HID_FRAME_MAX_BYTES;
}


#include "uhc.h"
