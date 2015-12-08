/*
  pitch_shift.c
  aleph - audio

  implement pitch shift using buffer, tap and echoTap data types

  single uninterpolated write head, dual 24.8 interpolated echoTap read heads

 */

#include "conversion.h"
#include "grain.h"
#include <stdlib.h>

// initialize with pointer to audio buffer
void grain_init(grain* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));

  echoTap_init(&(dl->echoTap), &(dl->tapWr));
  dl->echoTap.time = 256 * 50;
  dl->echoTap.speed = 256;
  dl->echoTap.edgeBehaviour = EDGE_WRAP;
  dl->echoTap.fadeLength = 0;
  dl->echoTap.min = 0;
  dl->echoTap.max = 256 * 1000;
  dl->tapWr.inc = 1;

  scrubTap_init(&(dl->scrubTap), &(dl->echoTap));
  dl->scrubTap.pitch = 256;
  dl->scrubTap.length = 256 * 25;
  dl->scrubTap.lengthNonRandom = 256 * 25;
  dl->scrubTap.fadeLength = 256 * 0;
  dl->scrubTap.randomise = 256 * 25;
  dl->scrubTap.time = 0;

}

fract32 grain_next(grain* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;
  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  
  readVal = scrubTap_read_xfade( &(dl->scrubTap));
  
  //DEBUG uncomment this line to check echoTap (ignoring any bugs in scrubTap)

  /* readVal = echoTap_read_xfade( &(dl->echoTap), 0); */

  buffer_tapN_next( &(dl->tapWr) );
  echoTap_next( &(dl->echoTap) );
  scrubTap_next( &(dl->scrubTap) );
  
  return readVal;
}

void grain_set_scrubPitch(grain* dl, s32 subsamples) {
  dl->scrubTap.pitch = subsamples;
}

//set length (24.8 time in samples)
void grain_set_scrubLength(grain* dl, s32 subsamples) {
  dl->scrubTap.lengthNonRandom = subsamples;
  dl->scrubTap.length = subsamples;
}

void  grain_set_scrubFadeLength(grain* dl, s32 subsamples) {
  dl->scrubTap.fadeLength = subsamples;
}

//set randomise (24.8 time in samples)
void grain_set_scrubRandomise(grain* dl, s32 subsamples) {
  dl->scrubTap.randomise = subsamples;
}

void grain_set_echoTime(grain* dl, s32 subsamples) {
  dl->echoTap.time = subsamples;
}

void  grain_set_echoFadeLength(grain* dl, s32 subsamples) {
    dl->echoTap.fadeLength = subsamples;
}

void grain_set_echoSpeed(grain* dl, s32 subsamples) {
  dl->echoTap.speed = subsamples;
}

void grain_set_echoEdgeBehaviour(grain* dl, u8 edgeBehaviour) {
  dl->echoTap.edgeBehaviour = edgeBehaviour;
}

void grain_set_echoMin(grain* dl, s32 subsamples) {
  dl->echoTap.min = subsamples;
}

void grain_set_echoMax(grain* dl, s32 subsamples) {
    dl->echoTap.max = subsamples;
}

void grain_set_writeEnable(grain* dl, s32 enable) {
  if (enable == 0)
    dl->tapWr.inc = 0;
  else
    dl->tapWr.inc = 1;
}
