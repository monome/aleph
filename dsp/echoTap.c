#include "echoTap.h"

#define SHORTEST_HALF_WAVE 250
#define LONGEST_DELAY 1500
// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->echo = 256*500;
  tap->idx_last = tapWr->idx;

  tap->echoMax = 256 * LONGEST_DELAY;

  //If inc == 0 doesn't move relative to write head we have a straight echo
  //If inc < 0 pitch shifts up
  //If 256>inc >0 pitch shifts down
  //If inc > 256 weird reversed pitch shift thing
  echoTap24_8_set_rate(tap, 256);
}

// increment the index in an echo
extern void echoTap24_8_next(echoTap24_8* tap){

    u8 zero_crossing = tap->zero_crossing;
    tap->echo = tap->echo + tap->inc;
    if (tap->echo<0 || (tap-> echo < SHORTEST_HALF_WAVE * 256 && zero_crossing )){
    //if (tap->echo<0 ){
        //tap->echo = tap->echoMax;
        tap->echo = 0;
        if(tap->inc <0)
            tap->inc = 512 - tap->inc;
    }
    else if (tap->echo > tap->echoMax ||(tap->echo > tap->echoMax - 256 * SHORTEST_HALF_WAVE && zero_crossing)){
    //else if (tap->echo > tap->echoMax ){
        //tap->echo = 0;
        if(tap->inc > 0)
            tap->inc = 512 - tap->inc;
    }
}

// interpolated read
#define SIGN_BIT 0x80000000
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
    return pan_lin_mix(samp1, samp2, inter_sample) ;
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
    tap->echoMax = echoMax;
}

// set 24.8 interp tap position directly in subsamples
extern void echoTap24_8_set_pos(echoTap24_8* tap, s32 echo){
    tap->echo = echo;
}
