/*
delay.h
aleph-audio

delay line object interface/

FIXME: delay is noninterpolated for now! there is a separate delayN class but we're not using it. should add buffer interpolation and then fix everything appropriately.

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
  /// FIXME: make these interpolated of course
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
} delayLine;

// initialize with pointer to audio buffer data
extern void delay_init(delayLine* dl, fract32* bufData, u32 frames);
// get next value given input
extern fract32 delay_next(delayLine* dl, fract32 in);
// set loop endpoint in seconds / samples
extern void delay_set_loop_sec(delayLine* dl, fix16 sec);
extern void delay_set_loop_samp(delayLine* dl, u32 samp);
// set delay time in seconds / samples
extern void delay_set_delay_sec(delayLine* dl, fix16 sec); 
extern void delay_set_delay_samp(delayLine* dl, u32 samp); 

// set read head rate
// extern void delay_set_rate(delayLine* dl, fix16 rate);

// set erase level
extern void delay_set_pre(delayLine* dl, fract32 pre);
// set write flag
extern void delay_set_write(delayLine* dl, u8 write);


// set read pos in seconds / samples
extern void delay_set_pos_read_sec(delayLine* dl, fix16 sec);
extern void delay_set_pos_read_samp(delayLine* dl, u32 samp);
// set write pos in seconds / samples
extern void delay_set_pos_write_sec(delayLine* dl, fix16 sec);
extern void delay_set_pos_write_samp(delayLine* dl, u32 samp);

// set read run flag
extern void delay_set_run_read(delayLine* dl, u8 val);
// set write run flag
extern void delay_set_run_write(delayLine* dl, u8 val);

// set read-head rate
extern void delay_set_rate(delayLine* dl, u32 val);

#endif // h guard
