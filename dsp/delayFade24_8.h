/*
delayFade24_8.h
aleph-audio

delay line object interface

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer16.h"
#include "fade.h"
#include "types.h"

// delay line data structure
// double-tapped delay line
typedef struct _delayFade24_8 {
  //-- audio buffer class
  audioBuffer16 buffer;
  //-- read and write taps (non-interpolated)
  buffer16Tap24_8 tapRd[2];
  buffer16Tap24_8 tapWr[2];
  // level of existing data to mix with new data
  // negative == 1
  fract16 preLevel;
  // flag to synchronize read tap with write tap
  u8 sync;
  // flag to toggle writing of new data
  u8 write;
  // flags to run or pause each tap (phasor)
  u8 runRd;
  u8 runWr;
  // crossfade parameters [0-1]
  fract16 fadeRd;
  fract16 fadeWr;

  /* // interpolaters */
  /* filter_1p_lo lpRdPan; */
  /* filter_1p_lo lpWrPan; */
  /* // fade targets */
  /* u32 fadeWr = 0; */
  /* u32 fadeRd = 0; */

} delayFade24_8;

// initialize with pointer to audio buffer data
extern void delayFade24_8_init(delayFade24_8* dl, volatile fract16* bufData, u32 frames);
// get next value given input
extern fract16 delayFade24_8_next(delayFade24_8* dl, fract16 in);
// set loop endpoint in seconds / samples
extern void delayFade24_8_set_loop_sec(delayFade24_8* dl, fix16 sec , u8 id );
extern void delayFade24_8_set_loop_samp(delayFade24_8* dl, u32 samp , u8 id );
// set delayFade24_8 time in seconds / samples
extern void delayFade24_8_set_delay_sec(delayFade24_8* dl, fix16 sec, u8 id );
extern void delayFade24_8_set_delay_samp(delayFade24_8* dl, u32 samp, u8 id );
// set read head rate
extern void delayFade24_8_set_readSpeed(delayFade24_8* dl, fix16 rate);
extern void delayFade24_8_set_writeSpeed(delayFade24_8* dl, fix16 rate);
// set erase level
extern void delayFade24_8_set_pre(delayFade24_8* dl, fract16 pre);
// set write flag
extern void delayFade24_8_set_write(delayFade24_8* dl, u8 write);


// set read pos in seconds / samples
extern void delayFade24_8_set_pos_read_sec(delayFade24_8* dl, fix16 sec , u8 id );
extern void delayFade24_8_set_pos_read_samp(delayFade24_8* dl, u32 samp , u8 id );
// set write pos in seconds / samples
extern void delayFade24_8_set_pos_write_sec(delayFade24_8* dl, fix16 sec , u8 id );
extern void delayFade24_8_set_pos_write_samp(delayFade24_8* dl, u32 samp , u8 id );

// set read run flag
extern void delayFade24_8_set_run_read(delayFade24_8* dl, u8 val);
// set write run flag
extern void delayFade24_8_set_run_write(delayFade24_8* dl, u8 val);

// set write level
extern void delayFade24_8_set_write(delayFade24_8* dl, u8 write);


#endif // h guard
