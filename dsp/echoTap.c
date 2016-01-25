#include <stdlib.h>
#include "echoTap.h"
#include "pan.h"

// intialize tap
void echoTap_init(echoTap* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->idx_last = tapWr->idx;

  tap->min = 0;
  tap->max = tapWr->loop << 7;
  tap->time = (tap->max + tap->min + 1 ) >> 1;
  tap->shape = SHAPE_TOPHAT;
  tap->edgeBehaviour = EDGE_WRAP;

  //256 subsamples = 1x in real time
  tap->speed = 256;
}

void echoTap_next(echoTap* tap){
  s32 inc = (tap->tapWr->inc << 8) - tap->speed;
  if(tap->time + inc <= tap->max && tap->time + inc >= tap->min )
    tap->time += inc;
  else {
    s32 echoRange;
    switch (tap->edgeBehaviour) {
    case EDGE_ONESHOT:
      if(tap->time +inc < tap->min)
	tap->time = tap->min;
      else
	tap->time = tap->max;
      break;
    case EDGE_BOUNCE:
      if(tap->time + inc < tap->min) {
	tap->speed = abs(tap->speed) * -1;
	tap->time = tap->min;
      }
      else if (tap->time + inc > tap->max){
	tap->speed = abs(tap->speed) * 1 ;
	tap->time = tap->max;
      }
      break;
    case EDGE_WRAP:
      echoRange = abs(tap->max - tap->min - tap->fadeLength);
      if (tap->time + inc > tap->max)
	tap->time -= echoRange;
      else if (tap->time + inc < tap->min)
	tap->time += echoRange;

      if (tap->time <= tap->max && tap->time >= tap->min)
	tap->time += tap->tapWr->inc*256 - tap->speed;
      break;
    default ://watch out! copy-pasted from edge_wrap
      echoRange = abs(tap->max - tap->min - tap->fadeLength);
      if (tap->time + inc > tap->max)
	tap->time -= echoRange;
      else if (tap->time + inc < tap->min)
	tap->time += echoRange;

      if (tap->time <= tap->max && tap->time >= tap->min)
	tap->time += inc;
      break;
    }
  }
}
    
fract32 echoTap_envelope(echoTap *tap) {
  s32 center = (tap->min + tap->max+1) >> 1;
  s32 dist_from_center = tap->time - center;
  if ( dist_from_center < 0 )
    dist_from_center *= -1;
  s32 max_dist_from_center = (tap->max - tap->min +1) >> 1 ;
  s32 scale_factor = FR32_MAX / max_dist_from_center;
  s32 amplitude;

  if( tap->shape ==SHAPE_TOPHAT) {
    amplitude = FR32_MAX;
  }
  else if( tap->shape ==SHAPE_TRIANGLE) {
    amplitude = dist_from_center * scale_factor;
    amplitude = FR32_MAX - amplitude;
  }
  else if( tap->shape ==SHAPE_HALFWAVE) {
    amplitude = dist_from_center * scale_factor;
    amplitude = mult_fr1x32x32(amplitude, amplitude);
    amplitude = FR32_MAX - amplitude;
  }
  else {
    amplitude = FR32_MAX;
  }
  return amplitude ;
}


fract32 echoTap_read_xfade(echoTap* echoTap, s32 offset) {
  s32 time, tapLength;
  fract32 fadeRatio;
  time = echoTap->time + offset;
  tapLength = echoTap->max - echoTap->min - echoTap->fadeLength;
  switch (echoTap->edgeBehaviour) {
  case EDGE_ONESHOT :
    time = echoTap->time;
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, echoTap->max - time);
      return equalPower_xfade ( 0, echoTap_read_interp_cubic(echoTap, time),
			       fadeRatio);
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, time - echoTap->min);
      return equalPower_xfade ( 0, echoTap_read_interp_cubic(echoTap, time),
				fadeRatio);
    }
    break;
  case EDGE_WRAP :
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, echoTap->max - time);
      return equalPower_xfade ( echoTap_read_interp_linear ( echoTap,
							     time - tapLength ),
				echoTap_read_interp_linear(echoTap, time),
				fadeRatio);
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, time - echoTap->min);
      return equalPower_xfade ( echoTap_read_interp_linear ( echoTap,
							     time + tapLength ),
				echoTap_read_interp_linear(echoTap, time),
				fadeRatio);
    }
    break;
  }
  return echoTap_read_interp_cubic(echoTap, time);
}

// set echo time directly in subsamples
extern void echoTap_set_pos(echoTap* tap, s32 time){
    tap->time = time;
}
