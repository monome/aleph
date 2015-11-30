#include <stdlib.h>
#include "scrubTap.h"
#include "pan.h"

// intialize tap
extern void scrubTap_init(scrubTap* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->idx_last = tapWr->idx;

  tap->scrubMin = 0;
  tap->scrubMax = 256 * tapWr->loop / 2;
  tap->scrubTime = (tap->scrubMax + tap->scrubMin + 1 )/2;
  tap->shape = SHAPE_TOPHAT;
  tap->edge_behaviour = EDGE_WRAP;

  //256 subsamples = 1x in real time
  tap->playback_speed = 256;
}

extern void scrubTap_next(scrubTap* tap){
  if(tap->scrubTime <= tap->scrubMax && tap->scrubTime >= tap->scrubMin )
    tap->scrubTime += tap->tapWr->inc*256 - tap->playback_speed;
  else {
    s32 scrubRange;
    switch (tap->edge_behaviour) {
    case EDGE_ONESHOT:
      tap->playback_speed = 0;
      break;
    case EDGE_WRAP:
      scrubRange = tap->scrubMax - tap->scrubMin;
      tap->scrubTime = abs (tap->scrubTime - tap->scrubMin + scrubRange)
	% scrubRange;
      tap->scrubTime += tap->scrubMin;
      break;
    case EDGE_BOUNCE:
      if(tap->scrubTime < tap->scrubMin) {
	tap->playback_speed = abs(tap->playback_speed) * -1 ;
      }
      else if (tap->scrubTime > tap->scrubMax) {
	tap->playback_speed = abs(tap->playback_speed) ;
      }
      tap->scrubTime += tap->tapWr->inc*256 - tap->playback_speed;
      break;
    }
  }
}

s32 scrubTap_envelope(scrubTap *tap){
  //FIXME need to make this thing crossfade like a state machine
  //which can either be fading out, fading in or not fading
  s32 center = (tap->scrubMin + tap->scrubMax+1) / 2;
  s32 dist_from_center = tap->scrubTime - center;
  if ( dist_from_center < 0 )
    dist_from_center *= -1;
  s32 max_dist_from_center = (tap->scrubMax - tap->scrubMin +1) / 2 ;
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
extern fract32 scrubTap_read_antialias(scrubTap* scrubTap){
    s32 num_samples = (scrubTap->playback_speed + 128) / 256;
    if( num_samples < 2 ) {
        return scrubTap_read_interp(scrubTap);
    }
    else if( num_samples >= MAX_ANTIALIAS ) {
        num_samples = MAX_ANTIALIAS;
    }

    s32 mix_factor = FR32_MAX / num_samples;
    fract32 pre_fader = 0;
    while(num_samples > 0) {
        s32 loop = scrubTap->tapWr->loop * 256;
        s32 idx = (scrubTap->tapWr->idx * 256 + loop
		   - scrubTap->scrubTime
		   - (num_samples -1) * 256)
	  % loop;
        u32 samp1_index = idx / 256;
        fract32 samp1 = scrubTap->tapWr->buf->data[samp1_index ];
        pre_fader = add_fr1x32 ( pre_fader, mult_fr1x32x32(samp1, mix_factor) );
        num_samples--;
    }
    s32 fader = scrubTap_envelope(scrubTap);
    fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader);
    return post_fader;
}

// interpolated read
extern fract32 scrubTap_read_interp(scrubTap* scrubTap){
    s32 loop = scrubTap->tapWr->loop * 256;
    s32 idx = (scrubTap->tapWr->idx * 256 + loop - scrubTap->scrubTime) % loop;

    u32 samp1_index = idx / 256;
    u32 samp2_index = ( (idx + 256) % loop ) / 256 ;

    fract32 samp1 = scrubTap->tapWr->buf->data[samp1_index ];
    fract32 samp2 = scrubTap->tapWr->buf->data[samp2_index ];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);
    u8 samp1_sign = abs(samp1) / samp1;
    u8 samp2_sign = abs(samp2) / samp2;
    scrubTap->zero_crossing = (samp1_sign != samp2_sign);

    fract32 pre_fader = pan_lin_mix(samp1, samp2, inter_sample)   ;
    s32 fader = scrubTap_envelope(scrubTap);
    fract32 post_fader = mult_fr1x32x32 ( pre_fader, fader);
    return post_fader;
}

// set scrub time directly in subsamples
extern void scrubTap_set_pos(scrubTap* tap, s32 scrubTime){
    tap->scrubTime = scrubTime;
}
