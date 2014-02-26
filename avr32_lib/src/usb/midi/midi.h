#ifndef _USB_MIDI_H_
#define _USB_MIDI_H_

#include "types.h"
#include "uhc.h"

extern u8 midiConnect;

// read and spawn events (non-blocking)
extern void midi_read(void);

// write to MIDI device
extern void midi_write(const u8* data, u32 bytes);

// MIDI device was plugged or unplugged
extern void midi_change(uhc_device_t* dev, u8 plug);

// main-loop setup routine for new device connection.
// this would be the place to perform any queries which require interrupts,
// and whose completion is necessary before polling.
// extern void midi_setup(void);



#endif
