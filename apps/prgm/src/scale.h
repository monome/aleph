#ifndef _ALEPH_PRGM_SCALE_H_
#define _ALEPH_PRGM_SCALE_H_

#include "types.h"

#define SCALE_JUST_SIZE 4096 //1536
#define FADE_SINE_BUF_SIZE 1024
#define FADE_SINE_BUF_SIZE_FIX (1024 << 16)

//set fade parameters
//extern fract32 table_lookup_idx(fract32 *tab, u32 size, fix16 idx);

//scale memory allocation
extern void scale_init(void);

//lookup and return scaled values
extern f32 just_lookup(s32 frq);
extern f32 clockspeed_lookup(s32 frq);
extern f32 straight_lookup(s32 frq);

#endif
