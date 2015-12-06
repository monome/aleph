#include <stdlib.h>
#include "echoTap.h"
#include "scrubTap.h"
#include "pan.h"
#include "noise.h"

// intialize tap
extern void scrubTap_init(scrubTap* tap, echoTap* echoTap){
  tap->echoTap = echoTap;

  tap->length = 1024;
  tap ->fadeLength = 512;
  tap->time = (tap->length  + 1 )/2;
  tap->idx_last = tap->time;

  //stationary scrubtap = stationary playback
  tap->pitch = 0;
  lcprng_reset(&(tap->randomGenerator), 0);
  tap->randomBw = 0;
}

#define simple_slew(x, y) x = (y + x * 50) / 50

extern s32 scrubTapRandom (scrubTap* tap) {
  return abs(simple_slew(tap->randomBw,
			 mult_fr1x32x32 (tap->randomise,
					 lcprng_next( &(tap->randomGenerator)))));
}

extern void scrubTap_next(scrubTap* tap){
  /* tap->length += scrubTapRandom(tap); */
  s32 tapRange = tap->length - tap->fadeLength;
  if(tap->time <= tap->length && tap->time >= 0 )
    //We aim to have echoTap->speed control tempo &
    //scrubTap->pitch control musical pitch
    tap->time += tap->echoTap->speed - tap->pitch;
  else {
    if (tap->time > tap->length)
      tap->time -= tapRange;
    else if (tap->time < 0)
      tap->time += tapRange;
    tap->time += tap->echoTap->speed - tap->pitch;
    tap->length = tap->lengthNonRandom
      + scrubTapRandom (tap);
  }
}

fract32 scrubTap_boundToFadeRatio (scrubTap* tap, s32 unbounded) {
  return max_num(0,
		 min_num (tap->fadeLength, unbounded))
    * (FR32_MAX / tap->fadeLength);
}

extern fract32 scrubTap_read_xfade (scrubTap* scrubTap) {
  s32 time = scrubTap->time;
  s32 ret = echoTap_read_xfade(scrubTap->echoTap, time);
  s32 tapLength = scrubTap->length - scrubTap->fadeLength;
  s32 fadeRatio;
  if (time > tapLength) {
    fadeRatio = scrubTap_boundToFadeRatio (scrubTap, scrubTap->length - time);
    ret = echoTap_xFade ( echoTap_read_xfade ( scrubTap->echoTap,
					       time - tapLength ),
			  ret,
			  fadeRatio);
  } else if (time < scrubTap->fadeLength) {
    fadeRatio = scrubTap_boundToFadeRatio (scrubTap, time);
    ret = echoTap_xFade ( echoTap_read_xfade ( scrubTap->echoTap,
					       time + tapLength ),
			  ret,
			  fadeRatio);
  }
  return ret;
}
