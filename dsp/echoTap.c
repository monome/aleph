#include <stdlib.h>
#include "echoTap.h"
#include "pan.h"

// intialize tap
extern void echoTap_init(echoTap* tap, bufferTapN* tapWr){
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

extern void echoTap_next(echoTap* tap){
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
  else if( tap->shape ==SHAPE_LUMP) {
    amplitude = dist_from_center * scale_factor;
    amplitude = mult_fr1x32x32(amplitude, amplitude);
    amplitude = FR32_MAX - amplitude;
  }
  else if (tap->shape == SHAPE_HALFWAVE) {
    s32 x_1 = dist_from_center * scale_factor;
    s32 x_2 = mult_fr1x32x32(x_1, x_1);
    s32 x_4 = mult_fr1x32x32(x_2, x_2);
    amplitude = (FR32_MAX - x_2) - (x_2 >> 2);
    amplitude = amplitude + (x_4 >> 2);
  }
  else {
    amplitude = FR32_MAX;
  }
  return amplitude ;
}

// interpolated read
inline extern fract32 echoTap_read_interp(echoTap* echoTap, s32 time){
    s32 loop = echoTap->tapWr->loop << 8;
    s32 idx = ((echoTap->tapWr->idx << 8) + loop - time) % loop;

    u32 samp1_index = idx / 256;
    u32 samp2_index = ( (idx + 256) % loop ) / 256 ;

    fract32 samp1 = echoTap->tapWr->buf->data[samp1_index ];
    fract32 samp2 = echoTap->tapWr->buf->data[samp2_index ];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);

    fract32 pre_fader = pan_lin_mix(samp1, samp2, inter_sample)   ;
    /* fract32 fader = echoTap_envelope(echoTap); */
    /* fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader); */
    return pre_fader;
}

//#!gnuplot
//plot cos(x*pi/2), sin(x*pi/2), 1 - x**2, 1 - (1 - x)**2
//fades x into y as pos slides from 0 to FR32_MAX
inline s32 equalPower_xfade (fract32 x, fract32 y, fract32 pos) {
  fract32 pos_2 = mult_fr1x32x32(pos, pos);
  fract32 amp_x = sub_fr1x32(FR32_MAX,  pos_2);
  fract32 pos_bar = sub_fr1x32(FR32_MAX,  pos);
  fract32 amp_y = sub_fr1x32(FR32_MAX,  mult_fr1x32x32(pos_bar, pos_bar));
  return add_fr1x32(mult_fr1x32x32(amp_x, x),
		    mult_fr1x32x32(amp_y, y));
}

inline fract32 echoTap_boundToFadeRatio (echoTap* tap, s32 unbounded) {
  return (fract32) (max_fr1x32 (0,
			     min_fr1x32 (tap->fadeLength, unbounded))
		    * (FR32_MAX / (max_fr1x32 (1, tap->fadeLength))));
}

inline fract32 echoTap_read_xfade(echoTap* echoTap, s32 offset) {
  s32 time, ret, tapLength;
  fract32 fadeRatio;
  time = echoTap->time + offset;
  ret = echoTap_read_interp(echoTap, time);
  tapLength = echoTap->max - echoTap->min - echoTap->fadeLength;
  switch (echoTap->edgeBehaviour) {
  case EDGE_ONESHOT :
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, echoTap->max - time);
      ret = equalPower_xfade ( 0, ret,
			       fadeRatio);
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, time - echoTap->min);
      ret = equalPower_xfade ( 0, ret,
			       fadeRatio);
    }
    break;
  case EDGE_WRAP :
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, echoTap->max - time);
      ret = equalPower_xfade ( echoTap_read_interp ( echoTap,
						     time - tapLength ),
			       ret,
			       fadeRatio);
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, time - echoTap->min);
      ret = equalPower_xfade ( echoTap_read_interp ( echoTap,
						     time + tapLength ),
			       ret,
			       fadeRatio);
    }
    break;
  case EDGE_BOUNCE :
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, echoTap->max - time);
      ret = equalPower_xfade (0, ret, fadeRatio);
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio = echoTap_boundToFadeRatio (echoTap, time - echoTap->min);
      ret = equalPower_xfade (0, ret, fadeRatio);
    }
    break;
  }
  return ret;
}

// set echo time directly in subsamples
extern void echoTap_set_pos(echoTap* tap, s32 time){
    tap->time = time;
}
