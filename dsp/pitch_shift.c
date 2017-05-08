/*
  pitch_shift.c
  aleph - audio

  implement pitch shift using buffer, tap and echoTap data types

  single uninterpolated write head, dual 24.8 interpolated echoTap read heads

 */

#include "conversion.h"
#include "pitch_shift.h"
#include <stdlib.h>


// initialize with pointer to audio buffer
void pitchShift_init(pitchShift* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));

  echoTap_init(&(dl->tapRd0), &(dl->tapWr));
  dl->tapRd0.edgeBehaviour = EDGE_WRAP;
  dl->tapRd0.shape = SHAPE_LUMP;
  dl->tapRd0.min = 0;
  dl->tapRd0.max = 256 * 1023;
  dl->tapRd0.time = 0;

  echoTap_init(&(dl->tapRd1), &(dl->tapWr));
  dl->tapRd1.edgeBehaviour = EDGE_WRAP;
  dl->tapRd1.shape = SHAPE_LUMP;
  dl->tapRd1.min = 0;
  dl->tapRd1.max = 256 * 1023;
  dl->tapRd1.time= 256 * 512;

  dl->pitchDetect = 0;

}

fract32 pitchShift_next(pitchShift* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;
  fract32 signalPeriod = pitchTrack(&(dl->pitchDetector), in);
  //DEBUG forcing detected Period to 1ms
  /* signalPeriod = 48 * 256; */
  fract32 desiredPitchShift = (fract32)dl->scrubCentrePitch;

  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  fract32 mix_factor = FR32_MAX / 2;
  readVal = mult_fr1x32x32(echoTap_read_interp( &(dl->tapRd0),
						dl->tapRd0.time) ,
			   mix_factor);
  readVal = add_fr1x32(readVal,
		       mult_fr1x32x32(echoTap_read_interp( &(dl->tapRd1),
							   dl->tapRd1.time ),
				      mix_factor));

  buffer_tapN_next( &(dl->tapWr) );

  //FIXME at the moment all the pitch shift logic happens in the echoTap functions.
  //That's no good! Should keep echoTap general to pitch shifter, sampler, chorus
  //(and any other short-delay type modules we can dream up)

  //FIXME so the pitch shift-specific logic should go here
  // basically when one read head passes the halfway point
  // kick the other one

  //FIXME at the moment this pitchshift is set out with
  //just two read heads with half-wave envelopes.
  //running in quadrature (amplitude sum always == 1)
  echoTap_next( &(dl->tapRd0) );
  echoTap_next( &(dl->tapRd1) );

  return readVal;
}

void pitchShift_set_pitchFactor(pitchShift* dl, s32 subsamples) {
  dl->tapRd0.speed = subsamples;
  dl->tapRd1.speed = subsamples;
}
