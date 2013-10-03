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

/* typedef union { s32 s; u8 b[4] } hidEventData; */
/* static hidEventData evData; */

// parse frame and spawn events
  /// FIXME: there should be a better way to do this.
  /// perhaps the event is triggered if any of the bytes change,
  /// and the 

void hid_gamepad_parse_frame(u8* data, u8 size) {
  u8 i;
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

#include "uhc.h"

