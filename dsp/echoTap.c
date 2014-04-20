#include "echoTap.h"

// intialize tap
extern void echoTap24_8_init(echoTap24_8* tap, bufferTapN* tapWr){
  tap->tapWr = tapWr;
  tap->echo = 0;
  tap->idx_last = tapWr->idx;

  tap->echoMax = 256 * 1000;

  //If inc == 0 doesn't move relative to write head we have a straight echo
  //If inc < 0 pitch shifts up
  //If 256>inc >0 pitch shifts down
  //If inc > 256 weird reversed pitch shift thing
  tap->inc = 0;
}

// increment the index in an echo
extern void echoTap24_8_next(echoTap24_8* tap){
    tap->echo = tap->echo + tap->inc;
    if (tap->echo<0){
        tap->echo = tap->echoMax;
    }
    else if (tap->echo > tap->echoMax){
        tap->echo = 0;
    }
}

// interpolated read
extern fract32 echoTap24_8_read(echoTap24_8* echoTap){
    s32 loop = echoTap->tapWr->loop * 256;
    s32 idx = (echoTap->tapWr->idx + loop - echoTap->echo) % loop;
    //loop and idx are the absolute position in subsamples of the desired read point

    //for now just return uninterpolated.  Will make no difference for harmonic pitch
    return echoTap->tapWr->buf->data[idx / 256];
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
extern void echoTap24_8_set_pos(echoTap24_8* tap, s32 echo){
    tap->echo = echo;
}
