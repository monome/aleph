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
  dl->scrubTap.edgeBehaviour = EDGE_WRAP;
  dl->scrubTap.shape = SHAPE_LUMP;
  dl->scrubTap.length = 256 * 1023;
  dl->scrubTap.time = 0;

  echoTap_init(&(dl->echoTap), &(dl->tapWr));
  dl->echoTap.edgeBehaviour = EDGE_WRAP;
  dl->echoTap.shape = SHAPE_LUMP;
  dl->echoTap.min = 0;
  dl->echoTap.max = 256 * 1023;
  dl->echoTap.time= 256 * 512;

}

fract32 grain_next(grain* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;
  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  fract32 mix_factor = FR32_MAX;
  
  readVal = mult_fr1x32x32(echoTap_read_interp( &(dl->scrubTap) ),
			   mix_factor);
  readVal = add_fr1x32(readVal,
		       mult_fr1x32x32(echoTap_read_interp( &(dl->echoTap) ),
				      mix_factor));

  buffer_tapN_next( &(dl->tapWr) );
  //scrubTap always drives the echoTap
  scrubTap_next( &(dl->scrubTap) );
  echoTap_next( &(dl->echoTap) );

  return readVal;
}


void  grain_set_scrubFadeLength(grain* dl, s32 subsamples) {
  dl->scrubTap.fadeLength = subsamples;
}

void  grain_set_echoFadeLength(grain* dl, s32 subsamples) {
    dl->echoTap.fadeLength = subsamples;
}

void grain_set_scrubPitch(grain* dl, s32 subsamples) {
  dl->scrubTap.pitch = subsamples;
}

//set length (24.8 time in samples)
void grain_set_scrubLength(grain* dl, s32 subsamples) {
  dl->scrubTap.length = subsamples;
}

//set randomise (24.8 time in samples)
void grain_set_scrubRandomise(grain* dl, s32 subsamples) {
  dl->scrubTap.randomise = subsamples;
}

//set scrubedgeBehaviour (24.8 time in samples)
void grain_set_scrubEdgeBehaviour(grain* dl, u8 edgeBehaviour) {
  //FIXME check whether edgeBehaviour value correct radix with serial debugging...
  dl->scrubTap.edgeBehaviour = edgeBehaviour;
}

void grain_set_echoTime(grain* dl, s32 subsamples) {
  dl->echoTap.time = subsamples;
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

void grain_set_echoLFOAmp(grain* dl, s32 subsamples) {
  //FIXME add this at the end once basic functionality works
}

void grain_set_echoLFOSpeed(grain* dl, s32 subsamples) {
  //FIXME add this at the end once basic functionality works
}
