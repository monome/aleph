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
  dl->tapWr.inc = 1;

  echoTap_init(&(dl->echoTap), &(dl->tapWr));
  dl->echoTap.time = 256 * 50;
  dl->echoTap.speed = 256;
  dl->echoTap.edgeBehaviour = EDGE_WRAP;
  dl->echoTap.fadeLength = 0;
  dl->echoTap.min = 0;
  dl->echoTap.max = 256 * 100;
  dl->echoTimeCountdown = -1;

  scrubTap_init(&(dl->scrubTap), &(dl->echoTap));
  dl->scrubCentrePitch = 128;
  dl->scrubLengthTarget = 256 * 48 * 25;

  pitchDetector_init(&(dl->pitchDetector));
  dl->pitchDetection = 1;

  dl->slewSpeed = FR32_MAX/1024;

}

fract32 grain_next(grain* dl, fract32 in, fract32 FM_signal) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;

  if (dl->echoTimeCountdown > 0) {
    dl->echoTap.time = simple_slew(dl->echoTap.time,
				   dl->echoTimeTarget,
				   SLEW_100MS );
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

  //DEBUG forcing nominal scrubLength to 50ms
  /* dl->scrubLengthTarget = 256 * 48 * 25; */
  
  fract32 signalPeriod = pitchTrack(&(dl->pitchDetector),
				    echoTap_read_xfade( &(dl->echoTap),
							0));
  //DEBUG forcing detected Period to 1ms
  /* signalPeriod = 48 * 256; */
  fract32 desiredPitchShift = (fract32) add_fr1x32((fract32)dl->scrubCentrePitch,
					      FM_signal);
  //DEBUG forcing desired pitchShift to 1 sample / sample 
  /* desiredPitchShift = 128; */
  //DEBUG force pitch detection enabled
  /* dl->pitchDetection = 1; */

  if (dl->pitchDetection == 1) {
    simple_slew(dl->scrubTap.length,
		/* signalPeriod * 10 */
		signalPeriod * (dl->scrubLengthTarget / max(1, signalPeriod)
				+ 1),
		SLEW_1MS);
  } else {
    simple_slew(dl->scrubTap.length,
		dl->scrubLengthTarget,
		SLEW_1MS);
  }
  //DEBUG forcing scrub tap length to 10ms
  /* dl->scrubTap.length = 256 * 48 * 10; */

  
  dl->scrubTap.frequency =
    - (FR32_MAX / 256 / 256 / 8) *
    mult_fr1x32x32((desiredPitchShift) * (FR32_MAX / 8 / 256),
		   FR32_MAX / max(dl->scrubTap.length, 1));
  //DEBUG with scrub tap length forced to 10ms and pitchshift forced to +1,
  //Should end up with a scrubTap Frequency of 100Hz
  /* printf("%d\t%d\t%d\t%d\n", dl->scrubTap.length, desiredPitchShift, dl->scrubTap.frequency, hzToDimensionless(100)); */

  //DEBUG forcing scrub tap rotation freq
  /* dl->scrubTap.frequency =  hzToDimensionless(-30); */

  scrubTap_next( &(dl->scrubTap) );

  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  
  readVal = scrubTap_read_xfade( &(dl->scrubTap));
  
  //DEBUG uncomment this line to check echoTap (ignoring any bugs in scrubTap)
  /* readVal = echoTap_read_xfade( &(dl->echoTap), 0); */

  //DEBUG uncomment this line to listen to the detected tone from pitch Tracker
  /* readVal = pitchTrackOsc(&(dl->pitchDetector)) / 10; */
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

void grain_disable_PitchDetection(grain* dl) {
  dl->pitchDetection = 0;
}

void grain_enable_PitchDetection(grain* dl) {
  dl->pitchDetection = 0;
}
