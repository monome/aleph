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

  dl->echoTimeCountdown = -1;
  dl->echoMaxCountdown = -1;
  dl->echoMinCountdown = -1;
  dl->echoFadeLengthCountdown = -1;

  dl->scrubFadeLengthCountdown = -1;
  dl->scrubLengthCountdown = -1;

}

#define simple_slew(x, y) x = (y + x * 1023) / 1024

#define grain_rottenSlew(param, target, countdown)	\
  if (countdown > 0) {					\
    param = simple_slew(param, target);			\
    countdown--;					\
  }

fract32 grain_next(grain* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;
  grain_rottenSlew(dl->echoTap.time, dl->echoTimeTarget, dl->echoTimeCountdown);
  grain_rottenSlew(dl->echoTap.fadeLength, dl->echoFadeLengthTarget, dl->echoFadeLengthCountdown);
  grain_rottenSlew(dl->echoTap.max, dl->echoMaxTarget, dl->echoMaxCountdown);
  grain_rottenSlew(dl->echoTap.min, dl->echoMinTarget, dl->echoMinCountdown);

  grain_rottenSlew(dl->scrubTap.fadeLength, dl->scrubFadeLengthTarget, dl->scrubFadeLengthCountdown);
  grain_rottenSlew(dl->scrubTap.lengthNonRandom, dl->scrubLengthTarget, dl->scrubLengthCountdown);
  //Maybe need the following too...
  /* grain_rottenSlew(dl->scrubTap.length, dl->scrubLengthTarget, dl->scrubLengthCountdown); */
  
  buffer_tapN_next( &(dl->tapWr) );
  echoTap_next( &(dl->echoTap) );
  scrubTap_next( &(dl->scrubTap) );

  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  
  readVal = scrubTap_read_xfade( &(dl->scrubTap));
  
  //DEBUG uncomment this line to check echoTap (ignoring any bugs in scrubTap)

  /* readVal = echoTap_read_xfade( &(dl->echoTap), 0); */

  return readVal;
}

#define param_poke(target, countdown) \
  target = subsamples;    \
  countdown = 20 * 48;

void grain_set_scrubPitch(grain* dl, s32 subsamples) {
  dl->scrubTap.pitch = subsamples;
}

void grain_set_scrubLength(grain* dl, s32 subsamples) {
  /* dl->scrubTap.lengthNonRandom = subsamples; */
  /* dl->scrubTap.length = subsamples; */
  param_poke(dl->scrubLengthTarget, dl->scrubLengthCountdown);
}

void  grain_set_scrubFadeLength(grain* dl, s32 subsamples) {
  /* dl->scrubTap.fadeLength = subsamples; */
  param_poke(dl->scrubFadeLengthTarget, dl->scrubFadeLengthCountdown);
}

//set randomise (24.8 time in samples)
void grain_set_scrubRandomise(grain* dl, s32 subsamples) {
  dl->scrubTap.randomise = (fract32) subsamples;
}

void grain_set_echoTime(grain* dl, s32 subsamples) {
  /* dl->echoTap.time = subsamples; */
  param_poke(dl->echoTimeTarget,  dl->echoTimeCountdown);
}

void  grain_set_echoFadeLength(grain* dl, s32 subsamples) {
  /* dl->echoTap.fadeLength = subsamples; */
  param_poke(dl->echoFadeLengthTarget, dl->echoFadeLengthCountdown);
}

void grain_set_echoSpeed(grain* dl, s32 subsamples) {
  dl->echoTap.speed = subsamples;
}

void grain_set_echoEdgeBehaviour(grain* dl, s32 edgeBehaviour) {
  dl->echoTap.edgeBehaviour = edgeBehaviour;
}

void grain_set_echoMin(grain* dl, s32 subsamples) {
  /* dl->echoTap.min = subsamples; */
  param_poke(dl->echoMinTarget, dl->echoMinCountdown);
}

void grain_set_echoMax(grain* dl, s32 subsamples) {
    /* dl->echoTap.max = subsamples; */
    param_poke(dl->echoMaxTarget, dl->echoMaxCountdown);
}

void grain_set_writeEnable(grain* dl, s32 enable) {
  if (enable == 0)
    dl->tapWr.inc = 0;
  else
    dl->tapWr.inc = 1;
}
