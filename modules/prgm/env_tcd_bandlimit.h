//  env_tcd_bandlimit.h
//  aleph

#ifndef aleph_env__tcd_bandlimit_h
#define aleph_env_tcd_bandlimit_h

#include "fix.h"
#include "fix32.h"
#include "types.h"

typedef struct _bandlimit {
    fract32 f;
    fract32 q;  //TEST!!!
    
    fract32 lp;
    fract32 hp;
    fract32 bp;
    
} bandlimit;


//init bandlimiting filter
extern void bandlimit_init(bandlimit *bl);

//set cutoff frequency
extern void bandlimit_set_f(bandlimit *bl, fract32 f);

//get next value
extern fract32 bandlimit_next(bandlimit *bl, fract32 input);


#endif
