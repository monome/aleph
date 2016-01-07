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
  dl->echoTap.max = 256 * 100;
  dl->tapWr.inc = 1;

  scrubTap_init(&(dl->scrubTap), &(dl->echoTap));
  pitchDetector_init(&(dl->pitchDetector));

  dl->echoTimeCountdown = -1;
  dl->scrubCentrePitch = hzToDimensionless (1000);

  dl->slewSpeed = FR32_MAX/1024;

}

fract32 grain_next(grain* dl, fract32 in, fract32 FM_signal) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;

  if (dl->echoTimeCountdown > 0) {
    dl->echoTap.time = simple_slew(dl->echoTap.time,
				   dl->echoTimeTarget,
				   FR32_MAX-65536 * 8 );
    //Debug uncomment the below line to check slew is strong enough
    /* dl->echoTap.time = dl->echoTimeTarget; */
    dl->echoTimeCountdown--;
  }
  
  simple_slew(dl->echoTap.max, dl->echoMaxTarget, dl->slewSpeed);
  simple_slew(dl->echoTap.min, dl->echoMinTarget, dl->slewSpeed);

  dl->echoTap.fadeLength =
    simple_slew(dl->echoTap.fadeLength,
		mult_fr1x32x32((fract32)dl->echoFadeLengthTarget,
			       sub_fr1x32((fract32) dl->echoMaxTarget,
					  (fract32) dl->echoMinTarget)),
		dl->slewSpeed);
  buffer_tapN_next( &(dl->tapWr) );
  echoTap_next( &(dl->echoTap) );
  
  fract32 signalFreq = pitchTrack(&(dl->pitchDetector),
				  echoTap_read_xfade( &(dl->echoTap), 0));
  fract32 desiredPitch = (fract32) add_fr1x32((fract32)dl->scrubCentrePitch,
					      FM_signal);
  desiredPitch = 100000;
  simple_slew(dl->scrubTap.frequency, - signalFreq / 2, 5000);
  /* dl->scrubTap.frequency = hzToDimensionless(100); */
  dl->scrubTap.length = desiredPitch * (FR32_MAX / dl->scrubTap.frequency) / 100;

  //DEBUG forcing scrub tap freq for now....
  /* dl->scrubTap.frequency =  hzToDimensionless(-30); */
  //DEBUG just fixing a sensible delay length for now...
  /* dl->scrubTap.length = 256 * 48 * 30; */

  scrubTap_next( &(dl->scrubTap) );

  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  
  readVal = scrubTap_read_xfade( &(dl->scrubTap));
  
  //DEBUG uncomment this line to check echoTap (ignoring any bugs in scrubTap)

  /* readVal = echoTap_read_xfade( &(dl->echoTap), 0); */

  return readVal;
}


void grain_set_scrubPitch(grain* dl, s32 subsamples) {
  dl->scrubCentrePitch = subsamples;
}

void grain_set_scrubLength(grain* dl, s32 subsamples) {
  /* dl->scrubTap.length = subsamples; */
  dl->scrubLengthTarget = subsamples;
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
