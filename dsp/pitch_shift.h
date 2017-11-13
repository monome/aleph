/*
delay.h
aleph-audio

delay line object interface/

  single uninterpolated write head, single 24.8 interpolated read head

 */
#ifndef _ALEPH_LINES_DELAYLINE_H_
#define _ALEPH_LINES_DELAYLINE_H_

#include "buffer.h"
#include "types.h"
#include "echoTap.h"

// delay line data structure
typedef struct _pitchShift {
  //-- audio buffer class
  audioBuffer buffer;

  echoTap tapRd0;
  echoTap tapRd1;
  bufferTapN tapWr;
  u8 pitchDetect;
  // flag to toggle writing of new data
  //u8 write;
  // level of existing data to mix with new data
  // negative == 1
  //fract32 preLevel;
  // flags to run or pause each tap (phasor)
  //u8 runRd;
  //u8 runWr;
} pitchShift;

// initialize with pointer to audio buffer data
extern void pitchShift_init(pitchShift* dl, fract32* bufData, u32 frames);

// get next value given input
extern fract32 pitchShift_next(pitchShift* dl, fract32 in);

//set pitchshift (24.8 fraction of original frequency)
void pitchShift_set_pitchFactor(pitchShift* dl, s32 subsamples) ;

#endif // h guard
