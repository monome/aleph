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
  dl->echoTap.time = 256 * 15;
  dl->echoTap.speed = 256;
  dl->echoTap.edgeBehaviour = EDGE_WRAP;
  dl->echoTap.fadeLength = 10;
  dl->echoTap.min = 0;
  dl->echoTap.max = 256 * 30;
  dl->tapWr.inc = 1;

  scrubTap_init(&(dl->scrubTap), &(dl->echoTap));
  dl->scrubTap.pitch = 256 * 2;
  dl->scrubTap.length = 256 * 10;
  dl->scrubTap.fadeLength = 256 * 5;
  dl->scrubTap.time = 0;

  dl->echoTimeCountdown = -1;

  dl->slewSpeed = FR32_MAX/1024;

}

#define simple_slew(x, y, slew) x = add_fr1x32( y,			\
						mult_fr1x32x32(slew,	\
							       sub_fr1x32(x, y) \
							       ))

fract32 grain_next(grain* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;
  if (dl->echoTimeCountdown > 0) {
    dl->echoTap.time = simple_slew(dl->echoTap.time,
				   dl->echoTimeTarget,
				   FR32_MAX-65536 );
    dl->echoTimeCountdown--;
  }

  simple_slew(dl->echoTap.fadeLength, dl->echoFadeLengthTarget, dl->slewSpeed);
  simple_slew(dl->echoTap.max, dl->echoMaxTarget, dl->slewSpeed);
  simple_slew(dl->echoTap.min, dl->echoMinTarget, dl->slewSpeed);

  simple_slew(dl->scrubTap.fadeLength, dl->scrubFadeLengthTarget, dl->slewSpeed);
  simple_slew(dl->scrubTap.length, dl->scrubLengthTarget, dl->slewSpeed);

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


void grain_set_scrubPitch(grain* dl, s32 subsamples) {
  dl->scrubTap.pitch = subsamples;
}

void grain_set_scrubLength(grain* dl, s32 subsamples) {
  /* dl->scrubTap.length = subsamples; */
  dl->scrubLengthTarget = subsamples;
}

void  grain_set_scrubFadeLength(grain* dl, s32 subsamples) {
  /* dl->scrubTap.fadeLength = subsamples; */
  dl->scrubFadeLengthTarget = subsamples;
}

void grain_set_echoTime(grain* dl, s32 subsamples) {
  /* dl->echoTap.time = subsamples; */
  dl->echoTimeTarget = subsamples;
  dl->echoTimeCountdown = 100 * 48;
}

void  grain_set_echoFadeLength(grain* dl, s32 subsamples) {
  /* dl->echoTap.fadeLength = subsamples; */
  dl->echoFadeLengthTarget = subsamples;
}

void grain_set_echoSpeed(grain* dl, s32 subsamples) {
  dl->echoTap.speed = subsamples;
}

void grain_set_echoEdgeBehaviour(grain* dl, s32 edgeBehaviour) {
  dl->echoTap.edgeBehaviour = edgeBehaviour;
}

void grain_set_echoMin(grain* dl, s32 subsamples) {
  /* dl->echoTap.min = subsamples; */
  dl->echoMinTarget = subsamples;
}

void grain_set_echoMax(grain* dl, s32 subsamples) {
    /* dl->echoTap.max = subsamples; */
  dl->echoMaxTarget = subsamples;
}

void grain_set_writeEnable(grain* dl, s32 enable) {
  if (enable == 0)
    dl->tapWr.inc = 0;
  else
    dl->tapWr.inc = 1;
}

void grain_set_slewSpeed(grain* dl, s32 newSpeed) {
  dl->slewSpeed = (fract32) newSpeed;
}
