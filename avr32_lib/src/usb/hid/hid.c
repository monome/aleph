/*
  hid.c
  aleph-avr32

  callbacks for HID gamepad
*/


#include "compiler.h"
#include "events.h"
#include "hid.h"

static u8 frame[HID_FRAME_MAX_BYTES];
static event_t ev = { .type = kEventHidPacket };


// set/unset dirty flag for given byte in frame
static void hid_set_byte_flag(u32* data, u8 byte, u8 val) {
  if(val) {
    (*data) |= (1 << byte);
  } else {
    (*data) &= 0xffffffff ^ (1 << byte);
  }
}

// test dirty flag for given byte in packet
u8 hid_get_byte_flag(u32 data, u8 byte) {
  return (data & (1 << byte)) > 0;
}

// parse frame and spawn events
void hid_parse_frame(u8* data, u8 size) {
  u8 i;
  // one event per frame changed.
  // event data is bitfield indicating bytes changed.
  for(i=0; i<size; i++) {
    hid_set_byte_flag( (u32*)&(ev.data), i, data[i] != frame[i] );
  }
  event_post(&ev);

  /* // one event per byte changed. */
  /* for(i=0; i<size; i++) { */
  /*   if(data[i] != frame[i]) { */
  /*     frame[i] = data[i]; */
  /*     ev.type = kEventHidPacket; */
      
  /*     /\* ev.data = frame[i]; *\/ */
  /*     /\* ev.data |= (i << 8); *\/ */
  /*     event_post(&ev); */
  /*   } */
  /* } */
}

// get the frame data 
const u8* hid_get_frame_data(void) {
  return (const u8*)frame;
}

const u8 hid_get_frame_size(void) {
  return HID_FRAME_MAX_BYTES;
}

#include "uhc.h"
