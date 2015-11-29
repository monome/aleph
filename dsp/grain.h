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
#include "scrubTap.h"

// delay line data structure
typedef struct _grain {
  //-- audio buffer class
  audioBuffer buffer;
  echoTap24_8 echoTap;
  scrubTap24_8 scrubTap;
  bufferTapN tapWr;
} grain;


// initialize with pointer to audio buffer data
extern void grain_init(grain* dl, fract32* bufData, u32 frames);

// get next value given input
extern fract32 grain_next(grain* dl, fract32 in);

//set scrubPitch (24.8 fraction of original frequency)
void grain_set_scrubPitch24_8(grain* dl, s32 subsamples);

//set scrubLength (24.8 time in samples)
void grain_set_scrubLength24_8(grain* dl, s32 subsamples);

//set scrubRandomise (24.8 time in samples)
void grain_set_scrubRandomise24_8(grain* dl, s32 subsamples);

//set scrubEdgeBehaviour (24.8 time in samples)
void grain_set_scrubEdgeBehaviour(grain* dl, u8 edgeBehaviour);

void grain_set_EchoTime24_8(grain* dl, s32 subsamples);

void grain_set_EchoSpeed24_8(grain* dl, s32 subsamples);

void grain_set_echoEdgeBehaviour(grain* dl, u8 edgeBehaviour);

void grain_set_echoMin24_8(grain* dl, s32 subsamples);

void grain_set_echoMax24_8(grain* dl, s32 subsamples);

void grain_set_echoLFOAmp24_8(grain* dl, s32 subsamples);

void grain_set_echoLFOSpeed24_8(grain* dl, s32 subsamples);

#endif // h guard
