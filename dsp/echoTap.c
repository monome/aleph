#include "echoTap.h"

#define SHORTEST_HALF_WAVE 256
#define LONGEST_DELAY 1500
// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->echo = 256*500;
  tap->idx_last = tapWr->idx;

  tap->echoMax = 256 * LONGEST_DELAY;
  tap->amplitude = FR32_MAX;

  //If inc == 0 doesn't move relative to write head we have a straight echo
  //If inc < 0 pitch shifts up
  //If 256>inc >0 pitch shifts down
  //If inc > 256 weird reversed pitch shift thing
  echoTap24_8_set_rate(tap, 256);
}

#define RAMP_UP  0x7FFF8
// increment the index in an echo
extern void echoTap24_8_next(echoTap24_8* tap){

    u8 zero_crossing = tap->zero_crossing;
    tap->echo = tap->echo + tap->inc;
    if (tap->echo<0  && tap->inc <0){
        tap->inc = 512 - tap->inc;
        tap->echo += tap->inc;
    }
    //FIXME Something fucked here - fudge factor on echoMax
    //Doesn't fully unfuck...
    //if (tap->echo > tap->echoMax*2 && tap->inc > 0){
    if (tap->echo > tap->echoMax*2 && tap->inc > 0){
        tap->inc = 512 - tap->inc;
        //tap->amplitude = 0;
        tap->echo += tap->inc;
    }
    s32 center = (tap->echoMax+1)/2;
    s32 dist_from_center =  tap->echo - center;
    s32 scaling_factor = FR32_MAX/center-0xFFF;
    tap->amplitude = dist_from_center * scaling_factor;
    tap->amplitude = mult_fr1x32x32(tap->amplitude, tap->amplitude);
    //tap->amplitude = FR32_MAX;

}

#define SIGN_BIT 0x80000000
// interpolated read
extern fract32 echoTap24_8_read(echoTap24_8* echoTap){
    s32 loop = echoTap->tapWr->loop * 256;
    s32 idx = (echoTap->tapWr->idx * 256 + loop - echoTap->echo) % loop;
    //loop and idx are the absolute position in subsamples of the desired read point

    //for now just return uninterpolated.  Will make no difference for harmonic pitch
    //return echoTap->tapWr->buf->data[idx / 256];
    u32 samp1_index = idx;
    u32 samp2_index = (idx + 256) % loop;

    fract32 samp1 = echoTap->tapWr->buf->data[samp1_index / 256];
    fract32 samp2 = echoTap->tapWr->buf->data[samp2_index / 256];
    fract32 inter_sample = FR32_MAX/256 * (idx % 256);
    u8 samp1_sign = samp1 & SIGN_BIT;
    u8 samp2_sign = samp2 & SIGN_BIT;
    echoTap->zero_crossing = (samp1_sign != samp2_sign);
    return mult_fr1x32x32(echoTap->amplitude, pan_lin_mix(samp1, samp2, inter_sample) );
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
extern void echoTap24_8_set_pos(echoTap24_8* tap, s32 echo){
    tap->echo = echo;
}
