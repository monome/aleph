//  env_tcd_bandlimit.c
//  aleph

#include "env_tcd_bandlimit.h"



static fract32 bandlimit_frame(bandlimit *bl, fract32 input);

void bandlimit_init(bandlimit *bl) {
    bl->f = 0;
    bl->q = 0x007cffff; //TEST!!!
    bl->lp = bl->hp = bl->bp = 0;
}


void bandlimit_set_f(bandlimit *bl, fract32 f) {
    if(bl->f != f) bl->f = f;
}


fract32 bandlimit_next(bandlimit *bl, fract32 input) {
    //process x2 and average
    fract32 tmp = shr_fr1x32(bandlimit_frame(bl, input), 1);
    tmp = add_fr1x32(tmp, shr_fr1x32(bandlimit_frame(bl, input), 1));
    return tmp;
}


fract32 bandlimit_frame(bandlimit *bl, fract32 input) {
    bl->lp = add_fr1x32(bl->lp, mult_fr1x32x32(bl->f, bl->bp));
    bl->hp = sub_fr1x32(sub_fr1x32(input, mult_fr1x32x32(bl->q, bl->bp)), bl->lp);
    bl->bp = add_fr1x32(bl->bp, mult_fr1x32x32(bl->f, bl->hp));
    return bl->bp;
}

