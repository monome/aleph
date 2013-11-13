/*
delayFadeN.h
aleph-audio

delay line object interface

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer.h"
#include "types.h"

// delay line data structure
typedef struct _delayFadeN {
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
} delayFadeN;

// initialize with pointer to audio buffer data
extern void delayFadeN_init(delayFadeN* dl, fract32* bufData, u32 frames);
// get next value given input
extern fract32 delayFadeN_next(delayFadeN* dl, fract32 in);
// set loop endpoint in seconds / samples
extern void delayFadeN_set_loop_sec(delayFadeN* dl, fix16 sec);
extern void delayFadeN_set_loop_samp(delayFadeN* dl, u32 samp);
// set delayFadeN time in seconds / samples
extern void delayFadeN_set_delayFadeN_sec(delayFadeN* dl, fix16 sec); 
extern void delayFadeN_set_delayFadeN_samp(delayFadeN* dl, u32 samp); 
// set read head rate
extern void delayFadeN_set_rate(delayFadeN* dl, fix16 rate);
// set erase level
extern void delayFadeN_set_pre(delayFadeN* dl, fract32 pre);
// set write flag
extern void delayFadeN_set_write(delayFadeN* dl, u8 write);


// set read pos in seconds / samples
extern void delayFadeN_set_pos_read_sec(delayFadeN* dl, fix16 sec);
extern void delayFadeN_set_pos_read_samp(delayFadeN* dl, u32 samp);
// set write pos in seconds / samples
extern void delayFadeN_set_pos_write_sec(delayFadeN* dl, fix16 sec);
extern void delayFadeN_set_pos_write_samp(delayFadeN* dl, u32 samp);

// set read run flag
extern void delayFadeN_set_run_read(delayFadeN* dl, u8 val);
// set write run flag
extern void delayFadeN_set_run_write(delayFadeN* dl, u8 val);



#endif // h guard
