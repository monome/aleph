#include <stdlib.h>
#include "echoTap.h"
#include "pan.h"

// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->echoTime = 256*500;
  tap->idx_last = tapWr->idx;

  tap->echoMin = 0;
  tap->echoMax = 256 * tapWr->loop;
  tap->grain_shape = GRAIN_TOPHAT;

  //If inc == 0 doesn't move relative to write head we have a straight echo
  //If inc < 0 pitch shifts up
  //If 256>inc >0 pitch shifts down
  //If inc > 256 weird reversed pitch shift thing
  echoTap24_8_set_rate(tap, 256);
}

extern void echoTap24_8_next(echoTap24_8* tap){
    if(tap->echoTime < tap->echoMax && tap->echoTime > tap->echoMin )
        tap->echoTime += tap->inc;
}
extern fract32 echoTap24_8_envelope(echoTap24_8 *tap){
    fract32 amplitude;
    s32 center, dist_from_center, scale_factor;
    switch(tap->grain_shape) {
        case GRAIN_TOPHAT:
            amplitude = FR32_MAX;
            break;
        case GRAIN_TRIANGLE:
            center = (tap->echoMin + tap->echoMax) / 2;
            dist_from_center = abs(tap->echoTime - center);
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
            break;
        case GRAIN_LUMP:
            center = (tap->echoMin + tap->echoMax) / 2;
            dist_from_center = abs(tap->echoTime - center);
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
        case GRAIN_FATLUMP:
            center = (tap->echoMin + tap->echoMax) / 2;
            dist_from_center = abs(tap->echoTime - center);
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
        case GRAIN_OBESELUMP:
            center = (tap->echoMin + tap->echoMax) / 2;
            dist_from_center = abs(tap->echoTime - center);
            scale_factor = FR32_MAX/(tap->echoMax - center);
            amplitude = dist_from_center * scale_factor;
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude = mult_fr1x32x32(amplitude, amplitude);
            amplitude =  sub_fr1x32 (FR32_MAX, amplitude);
        default:
            amplitude = FR32_MAX;
            break;
    }
    return amplitude;
}

// interpolated read
extern fract32 echoTap24_8_read(echoTap24_8* echoTap){
    s32 loop = echoTap->tapWr->loop * 256;
    s32 idx = (echoTap->tapWr->idx * 256 + loop - echoTap->echoTime) % loop;
    //loop and idx are the absolute position in subsamples of the desired read point

    //return echoTap->tapWr->buf->data[idx / 256];
    u32 samp1_index = idx;
    u32 samp2_index = (idx + 256) % loop;

    fract32 samp1 = echoTap->tapWr->buf->data[samp1_index / 256];
    fract32 samp2 = echoTap->tapWr->buf->data[samp2_index / 256];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);
    u8 samp1_sign = abs(samp1) / samp1;
    u8 samp2_sign = abs(samp2) / samp2;
    echoTap->zero_crossing = (samp1_sign != samp2_sign);
    return mult_fr1x32x32(echoTap24_8_envelope(&(echoTap)), pan_lin_mix(samp1, samp2, inter_sample) );
    //return negate_fr1x32(pan_lin_mix(samp1, samp2, inter_sample) );
}
/*
// interpolated read from arbitrary position
extern fract32 echoTap24_8_read_from(bufferTapN* tapWr, s32 idx){
}

// antialiased interpolated read
extern fract32 echoTap24_8_read_antialias(echoTap24_8* tap){
}
*/
// set rate - inputs
extern void echoTap24_8_set_rate(echoTap24_8 *tap, s32 inc){
    tap->inc = 256-inc;
}

// set 24.8 interp tap position directly in subsamples
extern void echoTap24_8_set_echoMax(echoTap24_8* tap, s32 echoMax){
    tap->echoMax = echoMax*256;
}

// set 24.8 interp tap position directly in subsamples
extern void echoTap24_8_set_pos(echoTap24_8* tap, s32 echoTime){
    tap->echoTime = echoTime;
}
