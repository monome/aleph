#ifndef _ALEPH_PRGM_SCALE_H_
#define _ALEPH_PRGM_SCALE_H_

#include "types.h"

#define SCALE_JUST_SIZE 4096 //1536

//scale memory allocation
extern void scale_init(void);

//lookup and return scaled values
extern f32 just_lookup(s32 frq);
extern f32 clockspeed_lookup(s32 frq);
extern f32 straight_lookup(s32 frq);

#endif
