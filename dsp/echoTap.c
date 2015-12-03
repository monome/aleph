#include <stdlib.h>
#include "echoTap.h"
#include "pan.h"

// intialize tap
extern void echoTap_init(echoTap* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->idx_last = tapWr->idx;

  tap->min = 0;
  tap->max = 256 * tapWr->loop / 2;
  tap->time = (tap->max + tap->min + 1 )/2;
  tap->shape = SHAPE_TOPHAT;
  tap->edgeBehaviour = EDGE_WRAP;

  //256 subsamples = 1x in real time
  tap->speed = 256;
}

extern void echoTap_next(echoTap* tap){
  if(tap->time <= tap->max && tap->time >= tap->min )
    tap->time += tap->tapWr->inc*256 - tap->speed;
  else {
    s32 echoRange;
    switch (tap->edgeBehaviour) {
    case EDGE_ONESHOT:
      if(tap->time < tap->min)
	tap->time = tap->min;
      else
	tap->time = tap->max;
      break;
    case EDGE_BOUNCE:
      if(tap->time < tap->min) {
	tap->speed = abs(tap->speed) * -1 ;
	tap->time += tap->fadeLength;
      }
      else {
	tap->speed = abs(tap->speed) ;
	tap->time -= tap->fadeLength;
      }
      tap->time += tap->tapWr->inc*256 - tap->speed;
      break;
    case EDGE_WRAP:
      echoRange = tap->max - tap->min - tap->fadeLength;
      if (tap->time >= tap->max)
	tap->time = tap->time - echoRange;
      else
	tap->time = tap->time + echoRange;
      tap->time += tap->tapWr->inc*256 - tap->speed;
      break;
    default ://watch out! copy-pasted from edge_wrap
      echoRange = tap->max - tap->min - tap->fadeLength;
      if (tap->time >= tap->max)
	tap->time = tap->time - echoRange;
      else
	tap->time = tap->time + echoRange;
      tap->time += tap->tapWr->inc*256 - tap->speed;
      break;
    }
  }
}
    
s32 echoTap_envelope(echoTap *tap) {
  s32 center = (tap->min + tap->max+1) / 2;
  s32 dist_from_center = tap->time - center;
  if ( dist_from_center < 0 )
    dist_from_center *= -1;
  s32 max_dist_from_center = (tap->max - tap->min +1) / 2 ;
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
    amplitude = (FR32_MAX - x_2) - x_2/4;
    amplitude = amplitude + x_4/4;
  }
  else {
    amplitude = FR32_MAX;
  }
  return amplitude ;
}

//FIXME done too much hacking around in here for this still to work...
// antialiased or interpolated read depending speed
/* extern fract32 echoTap_read_antialias(echoTap* echoTap){ */
/*     s32 num_samples = (echoTap->speed + 128) / 256; */
/*     if( num_samples < 2 ) { */
/*       return echoTap_read_interp(echoTap, echoTap->time); */
/*     } */
/*     else if( num_samples >= MAX_ANTIALIAS ) { */
/*         num_samples = MAX_ANTIALIAS; */
/*     } */

/*     s32 mix_factor = FR32_MAX / num_samples; */
/*     fract32 pre_fader = 0; */
/*     while(num_samples > 0) { */
/*         s32 loop = echoTap->tapWr->loop * 256; */
/*         s32 idx = (echoTap->tapWr->idx * 256 + loop */
/* 		   - echoTap->time */
/* 		   - (num_samples -1) * 256) */
/* 	  % loop; */
/*         u32 samp1_index = idx / 256; */
/*         fract32 samp1 = echoTap->tapWr->buf->data[samp1_index ]; */
/*         pre_fader = add_fr1x32 ( pre_fader, mult_fr1x32x32(samp1, mix_factor) ); */
/*         num_samples--; */
/*     } */
/*     //Debug disable envelope for now.  comment out below to re-enable */
/*     return  pre_fader; */
    
/*     s32 fader = echoTap_envelope(echoTap); */
/*     fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader); */
/*     return post_fader; */
/* } */

// interpolated read
extern fract32 echoTap_read_interp(echoTap* echoTap, s32 time){
    s32 loop = echoTap->tapWr->loop * 256;
    s32 idx = (echoTap->tapWr->idx * 256 + loop - time) % loop;

    u32 samp1_index = idx / 256;
    u32 samp2_index = ( (idx + 256) % loop ) / 256 ;

    fract32 samp1 = echoTap->tapWr->buf->data[samp1_index ];
    fract32 samp2 = echoTap->tapWr->buf->data[samp2_index ];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);
    u8 samp1_sign = abs(samp1) / samp1;
    u8 samp2_sign = abs(samp2) / samp2;
    echoTap->zero_crossing = (samp1_sign != samp2_sign);

    fract32 pre_fader = pan_lin_mix(samp1, samp2, inter_sample)   ;
    s32 fader = echoTap_envelope(echoTap);
    fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader);
    return post_fader;
}

//#!gnuplot
//plot cos(x*pi/2), sin(x*pi/2), 1 - x**2, 1 - (1 - x)**2
//fades x into y as pos slides from 0 to FR32_MAX
s32 echoTap_xFade (s32 x, s32 y, s32 pos) {
    s32 pos_2 = mult_fr1x32x32(pos, pos);
    s32 amp_x = sub_fr1x32(FR32_MAX,  pos_2);
    s32 pos_bar = sub_fr1x32(FR32_MAX,  pos);
    s32 amp_y = sub_fr1x32(FR32_MAX,  mult_fr1x32x32(pos_bar, pos_bar));
    return
      add_fr1x32(mult_fr1x32x32(amp_x, x),
		 mult_fr1x32x32(amp_y, y));
}

s32 max_num (s32 x, s32 y) {
  if ( x >= y)
    return x;
  else
    return y;
}

s32 min_num (s32 x, s32 y) {
  if ( x <= y)
    return x;
  else
    return y;
}

extern fract32 echoTap_read_xfade(echoTap* echoTap) {
  s32 time = echoTap->time;
  s32 ret = echoTap_read_interp(echoTap, time);
  s32 tapLength = echoTap->max - echoTap->min - echoTap->fadeLength;
  s32 fadeRatio;
  switch (echoTap->edgeBehaviour) {
  case EDGE_ONESHOT :
    fadeRatio = 0;
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio = min_num (echoTap->fadeLength, (echoTap->max - time))
	* (FR32_MAX / echoTap->fadeLength);
      ret = echoTap_xFade ( 0, ret,
			    fadeRatio);//Debug if xfades pop try reversing the sense of fade
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio =  min_num (echoTap->fadeLength, (time - echoTap->min))
	* (FR32_MAX / echoTap->fadeLength);

      ret = echoTap_xFade ( 0, ret,
			    fadeRatio);//Debug if xfades pop try reversing the sense of fade
    }
    break;
  case EDGE_WRAP :
    fadeRatio = 0;
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio = min_num (echoTap->fadeLength, (echoTap->max - time))
	* (FR32_MAX / echoTap->fadeLength);
      ret = echoTap_xFade ( echoTap_read_interp ( echoTap,
			    			  time - tapLength ),
			    ret,
			    fadeRatio);//Debug if xfades pop try reversing the sense of fade
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio =  min_num (echoTap->fadeLength, (time - echoTap->min))
	* (FR32_MAX / echoTap->fadeLength);
      ret = echoTap_xFade ( echoTap_read_interp ( echoTap,
			    			  time + tapLength ),
			    ret,
			    fadeRatio);//Debug if xfades pop try reversing the sense of fade
    }
    break;
  case EDGE_BOUNCE :
    if (time > echoTap->max - echoTap->fadeLength) {
      fadeRatio =  min_num (echoTap->fadeLength, (echoTap->max - time))
	* (FR32_MAX / echoTap->fadeLength);
      ret = echoTap_xFade ( // FIXME bouncing read-head gives wonky pitches
			       // Fading back from edges
			   echoTap_read_interp ( echoTap,
						 echoTap->max
						 - echoTap->fadeLength
						 + (echoTap->max - time)),
			    ret,
			    fadeRatio);//Debug if xfades pop try reversing the sense of fade
    } else if (time < echoTap->min + echoTap->fadeLength) {
      fadeRatio =  min_num (echoTap->fadeLength, (time - echoTap->min ))
	* (FR32_MAX / echoTap->fadeLength);
      ret = echoTap_xFade ( // FIXME bouncing read-head gives wonky pitches
			   // Fading back from edges
			   echoTap_read_interp ( echoTap,
						 echoTap->min
						 + echoTap->fadeLength
						 + (echoTap->min - time)),
			    ret,
			    fadeRatio);
    }
    break;
  }
  return ret;
}

// set echo time directly in subsamples
extern void echoTap_set_pos(echoTap* tap, s32 time){
    tap->time = time;
}
