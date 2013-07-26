/*
delay.h
aleph-audio

delay line object interface

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer.h"
#include "types.h"

// delay line data structure
typedef struct _delayLine {
  //-- audio buffer class
  audioBuffer buffer;
  //-- read and write taps (non-interpolated)
  bufferTapN tapRd;
  bufferTapN tapWr;
  // flag to synchronize read tap with write tap
  u8 sync;
  // write level
  fract32 writeLevel;
  // erase level
  fract32 eraseLevel;
} delayLine;

// initialize with pointer to audio buffer data
extern void delay_init(delayLine* dl, fract32* bufData, u32 frames);
// get next value given input
extern fract32 delay_next(delayLine* dl, fract32 in);
// set loop endpoint in seconds
extern void delay_set_loop(delayLine* dl, fix16 sec);
// set delay time in seconds
extern void delay_set_delay(delayLine* dl, fix16 sec); 
// set synchronization flag
// extern void delay_set_sync(delayLine* dl, u8 sync);
// set erase level
extern void delay_set_erase(delayLine* dl, fract32 erase);
// set write level
extern void delay_set_write(delayLine* dl, fract32 write);
// set read head rate
extern void delay_set_rate(delayLine* dl, fix16 rate);

#endif // h guard
