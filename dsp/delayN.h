/*
delayN.h
aleph-audio

delay line object interface

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer.h"
#include "types.h"

// delay line data structure
typedef struct _delayN {
  //-- audio buffer class
  audioBuffer buffer;
  //-- read and write taps (non-interpolated)
  bufferTapN tapRd;
  bufferTapN tapWr;
  // flag to synchronize read tap with write tap
  u8 sync;
  // flag to toggle writing of new data
  u8 write;
  // level of existing data to mix with new data
  // negative == 1
  fract32 preLevel;
  // flags to run or pause each tap (phasor)
  u8 runRd;
  u8 runWr;
} delayN;

// initialize with pointer to audio buffer data
extern void delayN_init(delayN* dl, fract32* bufData, u32 frames);
// get next value given input
extern fract32 delayN_next(delayN* dl, fract32 in);
// set loop endpoint in seconds / samples
extern void delayN_set_loop_sec(delayN* dl, fix16 sec);
extern void delayN_set_loop_samp(delayN* dl, u32 samp);
// set delayN time in seconds / samples
extern void delayN_set_delayN_sec(delayN* dl, fix16 sec); 
extern void delayN_set_delayN_samp(delayN* dl, u32 samp); 
// set read head rate
//extern void delayN_set_rate(delayN* dl, fix16 rate);
// set erase level
extern void delayN_set_pre(delayN* dl, fract32 pre);
// set write level
extern void delayN_set_write(delayN* dl, u8 write);


// set read pos in seconds / samples
extern void delayN_set_pos_read_sec(delayN* dl, fix16 sec);
extern void delayN_set_pos_read_samp(delayN* dl, u32 samp);
// set write pos in seconds / samples
extern void delayN_set_pos_write_sec(delayN* dl, fix16 sec);
extern void delayN_set_pos_write_samp(delayN* dl, u32 samp);

// set read run flag
extern void delayN_set_run_read(delayN* dl, u8 val);
// set write run flag
extern void delayN_set_run_write(delayN* dl, u8 val);

// set read-head rate multiplier
extern void delayFadeN_set_mul(delayFadeN* dl, u32 val , u8 id );
// set read-head rate divider
extern void delayFadeN_set_div(delayFadeN* dl, u32 val , u8 id );


#endif // h guard
