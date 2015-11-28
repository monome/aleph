#include <stdlib.h>
#include "echoTap.h"
#include "pan.h"

// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->idx_last = tapWr->idx;

  tap->echoMin = 0;
  tap->echoMax = 256 * tapWr->loop / 2;
  tap->echoTime = (tap->echoMax + tap->echoMin + 1 )/2;
  tap->shape = SHAPE_TOPHAT;
  tap->edge_behaviour = EDGE_WRAP;

  //256 subsamples = 1x in real time
  tap->playback_speed = 256;
}

extern void echoTap24_8_next(echoTap24_8* tap){
  if(tap->echoTime <= tap->echoMax && tap->echoTime >= tap->echoMin )
    tap->echoTime += tap->tapWr->inc*256 - tap->playback_speed;
  else {
    s32 echoRange;
    switch (tap->edge_behaviour) {
    case EDGE_ONESHOT:
      tap->playback_speed = 0;
      break;
    case EDGE_WRAP:
      echoRange = tap->echoMax - tap->echoMin;
      tap->echoTime = abs (tap->echoTime - tap->echoMin + echoRange)
	% echoRange;
      tap->echoTime += tap->echoMin;
      break;
    case EDGE_BOUNCE:
      if(tap->echoTime < tap->echoMin) {
	tap->playback_speed = abs(tap->playback_speed) * -1 ;
      }
      else if (tap->echoTime > tap->echoMax) {
	tap->playback_speed = abs(tap->playback_speed) ;
      }
      tap->echoTime += tap->tapWr->inc*256 - tap->playback_speed;
      break;
    }
  }
}

s32 echoTap24_8_envelope(echoTap24_8 *tap){
  //FIXME need to make this thing crossfade like a state machine
  //which can either be fading out, fading in or not fading
  s32 center = (tap->echoMin + tap->echoMax+1) / 2;
  s32 dist_from_center = tap->echoTime - center;
  if ( dist_from_center < 0 )
    dist_from_center *= -1;
  s32 max_dist_from_center = (tap->echoMax - tap->echoMin +1) / 2 ;
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

// antialiased read
extern fract32 echoTap24_8_read_antialias(echoTap24_8* echoTap){
    s32 num_samples = (echoTap->playback_speed + 128) / 256;
    if( num_samples < 2 ) {
        return echoTap24_8_read_interp(echoTap);
    }
    else if( num_samples >= MAX_ANTIALIAS ) {
        num_samples = MAX_ANTIALIAS;
    }

    s32 mix_factor = FR32_MAX / num_samples;
    fract32 pre_fader = 0;
    while(num_samples > 0) {
        s32 loop = echoTap->tapWr->loop * 256;
        s32 idx = (echoTap->tapWr->idx * 256 + loop
		   - echoTap->echoTime
		   - (num_samples -1) * 256)
	  % loop;
        u32 samp1_index = idx / 256;
        fract32 samp1 = echoTap->tapWr->buf->data[samp1_index ];
        pre_fader = add_fr1x32 ( pre_fader, mult_fr1x32x32(samp1, mix_factor) );
        num_samples--;
    }
    s32 fader = echoTap24_8_envelope(echoTap);
    fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader);
    return post_fader;
}

// interpolated read
extern fract32 echoTap24_8_read_interp(echoTap24_8* echoTap){
    s32 loop = echoTap->tapWr->loop * 256;
    s32 idx = (echoTap->tapWr->idx * 256 + loop - echoTap->echoTime) % loop;

    u32 samp1_index = idx / 256;
    u32 samp2_index = ( (idx + 256) % loop ) / 256 ;

    fract32 samp1 = echoTap->tapWr->buf->data[samp1_index ];
    fract32 samp2 = echoTap->tapWr->buf->data[samp2_index ];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);
    u8 samp1_sign = abs(samp1) / samp1;
    u8 samp2_sign = abs(samp2) / samp2;
    echoTap->zero_crossing = (samp1_sign != samp2_sign);

    fract32 pre_fader = pan_lin_mix(samp1, samp2, inter_sample)   ;
    s32 fader = echoTap24_8_envelope(echoTap);
    fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader);
    return post_fader;
}

// set echo time directly in subsamples
extern void echoTap24_8_set_pos(echoTap24_8* tap, s32 echoTime){
    tap->echoTime = echoTime;
}
