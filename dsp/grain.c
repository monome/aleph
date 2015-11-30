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

  echoTap_init(&(dl->scrubTap), &(dl->tapWr));
  dl->scrubTap.edge_behaviour = EDGE_WRAP;
  dl->scrubTap.shape = SHAPE_LUMP;
  dl->scrubTap.scrubMin = 0;
  dl->scrubTap.scrubMax = 256 * 1023;
  dl->scrubTap.scrubTime = 0;

  echoTap_init(&(dl->echoTap), &(dl->tapWr));
  dl->echoTap.edge_behaviour = EDGE_WRAP;
  dl->echoTap.shape = SHAPE_LUMP;
  dl->echoTap.echoMin = 0;
  dl->echoTap.echoMax = 256 * 1023;
  dl->echoTap.echoTime= 256 * 512;

}

fract32 grain_next(grain* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;
  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  fract32 mix_factor = FR32_MAX / 2;
  
  readVal = mult_fr1x32x32(echoTap_read_interp( &(dl->scrubTap) ),
			   mix_factor);
  readVal = add_fr1x32(readVal,
		       mult_fr1x32x32(echoTap_read_interp( &(dl->echoTap) ),
				      mix_factor));

  buffer_tapN_next( &(dl->tapWr) );
  //scrubTap always drives the echoTap
  scrubTap_next( &(dl->scrubTap) );

  return readVal;
}

void grain_set_scrubPitch(grain* dl, s32 subsamples) {
  dl->scrubTap.playback_speed = subsamples;
  dl->echoTap.playback_speed = subsamples;
}

//set scrubLength (24.8 time in samples)
void grain_set_scrubLength(grain* dl, s32 subsamples) {
}

//set scrubRandomise (24.8 time in samples)
void grain_set_scrubRandomise(grain* dl, s32 subsamples) {
}

//set scrubEdgeBehaviour (24.8 time in samples)
void grain_set_scrubEdgeBehaviour(grain* dl, u8 edgeBehaviour) {
}

void grain_set_echoTime(grain* dl, s32 subsamples) {
}

void grain_set_echoSpeed(grain* dl, s32 subsamples) {
}

void grain_set_echoEdgeBehaviour(grain* dl, u8 edgeBehaviour) {
}

void grain_set_echoMin(grain* dl, s32 subsamples) {
}

void grain_set_echoMax(grain* dl, s32 subsamples) {
}

void grain_set_echoLFOAmp(grain* dl, s32 subsamples) {
}

void grain_set_echoLFOSpeed(grain* dl, s32 subsamples) {
}
