/*
  hid.c
  aleph-avr32

  callbacks for HID gamepad
*/


#include "compiler.h"
#include "events.h"
#include "hid.h"

static u8 frame[HID_FRAME_MAX_BYTES];
static u32 dirty;
static u8 size;

// app polls
// static event_t ev = { .type = kEventHidPacket };

// set/unset dirty flag for given byte in frame
/* static inline void hid_set_byte_flag(u32* data, u8 byte, u8 val) { */
/*   if(val) { */
/*     (*data) |= (1 << byte); */
/*   } else { */
/*     (*data) &= 0xffffffff ^ (1 << byte); */
/*   } */
/* } */

// test dirty flag for given byte in packet
//u8 hid_get_byte_flag(u32 data, u8 byte) {
u8 hid_get_byte_flag(u8 byte) {
  return (dirty & (1 << byte)) > 0;
}

// parse frame and spawn events
void hid_parse_frame(u8* data, u8 sz) {
  u8 b;
  // one event per frame changed.
  // event data is bitfield indicating bytes changed.
  //  dirty = 0x00000000;
  size = sz;
  for(b=0; b<size; b++) {
    //    hid_set_byte_flag( (u32*)&(ev.data), i, data[i] != frame[i] );
    if(*data) {
      dirty |= (1 << b);
    } else {
      dirty &= 0xffffffff ^ (1 << b);
    }
    frame[b] = *data;
    data++;
  }

  /// let the application poll
  //  event_post(&ev);
}

// get the frame data 
const volatile u8* hid_get_frame_data(void) {
  return (const volatile u8*)frame;
}

const volatile u8 hid_get_frame_size(void) {
  return HID_FRAME_MAX_BYTES;
}

const volatile u32 hid_get_frame_dirty(void) {
  return (const volatile u32)dirty;
}

// HID device was plugged or unplugged
extern void hid_change(uhc_device_t* dev, u8 plug) {
  event_t e;
  if(plug) { 
    e.type = kEventHidConnect; 
  } else {
    e.type = kEventHidDisconnect;
  }
  // posting an event so the main loop can respond
  event_post(&e); 
}

//... ASF style choice??? :S
#include "uhc.h"
