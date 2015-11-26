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

  echoTap24_8_init(&(dl->tapRd0), &(dl->tapWr));
  dl->tapRd0.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd0.shape = SHAPE_LUMP;
  dl->tapRd0.echoMin = 0;
  dl->tapRd0.echoMax = 256 * 1023;
  dl->tapRd0.echoTime = 0;

  echoTap24_8_init(&(dl->tapRd1), &(dl->tapWr));
  dl->tapRd1.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd1.shape = SHAPE_LUMP;
  dl->tapRd1.echoMin = 0;
  dl->tapRd1.echoMax = 256 * 1023;
  dl->tapRd1.echoTime= 256 * 512;

}

fract32 pitchShift_next(pitchShift* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;

  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  fract32 mix_factor = FR32_MAX / 2;
  readVal = mult_fr1x32x32(echoTap24_8_read_interp( &(dl->tapRd0) ),
			   mix_factor);
  readVal = add_fr1x32(readVal,
		       mult_fr1x32x32(echoTap24_8_read_interp( &(dl->tapRd1) ),
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
  echoTap24_8_next( &(dl->tapRd0) );
  echoTap24_8_next( &(dl->tapRd1) );

  return readVal;
}

void pitchShift_set_pitchFactor24_8(pitchShift* dl, s32 subsamples) {
  dl->tapRd0.playback_speed = subsamples;
  dl->tapRd1.playback_speed = subsamples;
  s32 scan_speed = abs (dl->tapRd0.playback_speed - 256);
}
