#include <stdlib.h>
#include "echoTap.h"
#include "scrubTap.h"
#include "pan.h"

// intialize tap
extern void scrubTap_init(scrubTap* tap, echoTap* echoTap){
  tap->echoTap = echoTap;

  tap->length = 1024;
  tap ->fadeLength = 512;
  tap->time = (tap->length  + 1 )/2;
  tap->idx_last = tap->time;

  //stationary scrubtap = stationary playback
  tap->pitch = 0;
}

extern void scrubTap_next(scrubTap* tap){
  if(tap->time <= tap->length && tap->time >= 0 )
    //We aim to have echoTap->speed control tempo &
    //scrubTap->pitch control musical pitch
    tap->time += tap->echoTap->speed - tap->pitch;
  else {
    s32 tapRange = tap->length - tap->fadeLength;
    if (tap->time >= tap->length)
      tap->time = tap->time - tapRange;
    else
      tap->time = tap->time + tapRange;
    tap->time += tap->echoTap->speed - tap->pitch;
  }
}

extern fract32 scrubTap_read_xfade (scrubTap* scrubTap) {
  s32 time = scrubTap->time;
  s32 ret = echoTap_read_xfade(scrubTap->echoTap, time);
  s32 tapLength = scrubTap->length - scrubTap->fadeLength;
  s32 fadeRatio;
  if (time > scrubTap->length - scrubTap->fadeLength) {
    fadeRatio = min_num (scrubTap->fadeLength, (scrubTap->length - time))
      * (FR32_MAX / scrubTap->fadeLength);
    ret = echoTap_xFade ( echoTap_read_xfade ( scrubTap->echoTap,
					       time - tapLength ),
			  ret,
			  fadeRatio);//Debug if xfades pop try reversing the sense of fade
  } else if (time < scrubTap->fadeLength) {
    fadeRatio =  min_num (scrubTap->fadeLength, time)
      * (FR32_MAX / scrubTap->fadeLength);
    ret = echoTap_xFade ( echoTap_read_xfade ( scrubTap->echoTap,
					       time + tapLength ),
			  ret,
			  fadeRatio);//Debug if xfades pop try reversing the sense of fade
  }
  return ret;
}
