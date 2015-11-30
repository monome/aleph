#include <stdlib.h>
#include "echoTap.h"
#include "scrubTap.h"
#include "pan.h"

// intialize tap
extern void scrubTap_init(scrubTap* tap, echoTap* echoTap){
  tap->echoTap = echoTap;

  tap->length = 1024;
  tap->time = (tap->length  + 1 )/2;
  tap->idx_last = tap->time;

  tap->shape = SHAPE_TOPHAT;
  tap->edgeBehaviour = EDGE_WRAP;

  //256 subsamples = 1x in real time
  tap->pitch = 256;
}

extern void scrubTap_next(scrubTap* tap){
  if(tap->time <= tap->length && tap->time >= 0 )
    //We aim to have echoTap->speed control tempo &
    //scrubTap->pitch control musical pitch
    tap->time += tap->echoTap->speed - tap->pitch;
  else {
    switch (tap->edgeBehaviour) {
    case EDGE_WRAP:
      tap->time = abs (tap->time + tap->length)
	% tap->length;
      break;
    case EDGE_BOUNCE:
      if(tap->time < 0) {
	tap->pitch = abs(tap->pitch) * -1 ;
      }
      else if (tap->time > tap->length) {
	tap->pitch = abs(tap->pitch) ;
      }
      tap->time += tap->echoTap->speed - tap->pitch;
      break;
    default ://watch out! copy-pasted from edge_wrap
      tap->time = abs (tap->time + tap->length)
	% tap->length;
      break;

    }
  }
}

// antialiased read
extern fract32 scrubTap_read_antialias(scrubTap* scrubTap){
    s32 num_samples = (scrubTap->pitch + 128) / 256;
    if( num_samples < 2 ) {
        return scrubTap_read_interp(scrubTap);
    }
    else if( num_samples >= MAX_ANTIALIAS ) {
        num_samples = MAX_ANTIALIAS;
    }

    s32 mix_factor = FR32_MAX / num_samples;
    fract32 pre_fader = 0;
    while(num_samples > 0) {
        s32 loop = scrubTap->echoTap->tapWr->loop * 256;
        s32 idx = (scrubTap->echoTap->tapWr->idx * 256 + loop
		   - (scrubTap->time + scrubTap->echoTap->time)
		   - (num_samples -1) * 256)
	  % loop;
        u32 samp1_index = idx / 256;
        fract32 samp1 = scrubTap->echoTap->tapWr->buf->data[samp1_index];
        pre_fader = add_fr1x32 ( pre_fader, mult_fr1x32x32(samp1, mix_factor) );
        num_samples--;
    }
    return pre_fader;
}

// interpolated read
extern fract32 scrubTap_read_interp(scrubTap* scrubTap){
    s32 loop = scrubTap->echoTap->tapWr->loop * 256;
    s32 idx = (scrubTap->echoTap->tapWr->idx * 256 + loop - scrubTap->time) % loop;

    u32 samp1_index = idx / 256;
    u32 samp2_index = ( (idx + 256) % loop ) / 256 ;

    fract32 samp1 = scrubTap->echoTap->tapWr->buf->data[samp1_index ];
    fract32 samp2 = scrubTap->echoTap->tapWr->buf->data[samp2_index ];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);
    u8 samp1_sign = abs(samp1) / samp1;
    u8 samp2_sign = abs(samp2) / samp2;
    scrubTap->zero_crossing = (samp1_sign != samp2_sign);

    fract32 pre_fader = pan_lin_mix(samp1, samp2, inter_sample)   ;
    return pre_fader;
}
