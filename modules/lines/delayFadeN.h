/*
delayFadeN.h
aleph-audio

delay line object interface

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer.h"
#include "fade.h"
#include "types.h"

// delay line data structure
// double-tapped delay line
typedef struct _delayFadeN {
  //-- audio buffer class
  audioBuffer buffer;
  //-- read and write taps (non-interpolated)
  bufferTapN tapRd[2];
  bufferTapN tapWr;
  // level of existing data to mix with new data
  // negative == 1
  fract32 preLevel;
  // flag to synchronize read tap with write tap
  u8 sync;
  // flag to toggle writing of new data
  u8 write;
  // flags to run or pause each tap (phasor)
  u8 runRd;
  u8 runWr;
  // crossfade parameters [0-1]
  fract32 fadeRd;
  fract32 fadeWr;
  
} delayFadeN;

// initialize with pointer to audio buffer data
extern void delayFadeN_init(delayFadeN* dl, volatile fract32* bufData, u32 frames);
// get next value given input
extern fract32 delayFadeN_next(delayFadeN* dl, fract32 in);
// set loop endpoint in seconds / samples
extern void delayFadeN_set_loop_ms(delayFadeN* dl, fract32 ms);
// set delayFadeN time in seconds / samples
extern void delayFadeN_set_delay_ms(delayFadeN* dl, fract32 ms, u8 id );
// set erase level
extern void delayFadeN_set_pre(delayFadeN* dl, fract32 pre);
// set write flag
extern void delayFadeN_set_write(delayFadeN* dl, u8 write);

// set read pos in seconds / samples
extern void delayFadeN_set_pos_read_ms(delayFadeN* dl, fract32 ms, u8 id );
// set write pos in seconds / samples
extern void delayFadeN_set_pos_write_ms(delayFadeN* dl, fract32 ms);

// set read run flag
extern void delayFadeN_set_run_read(delayFadeN* dl, u8 val);
// set write run flag
extern void delayFadeN_set_run_write(delayFadeN* dl, u8 val);

// set read-head rate multiplier
extern void delayFadeN_set_mul(delayFadeN* dl, u32 val);
// set read-head rate divider
extern void delayFadeN_set_div(delayFadeN* dl, u32 val);

// set erase level
extern void delayFadeN_set_pre(delayFadeN* dl, fract32 pre);
// set write level
extern void delayFadeN_set_write(delayFadeN* dl, u8 write);


#endif // h guard
