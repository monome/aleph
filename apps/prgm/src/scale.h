#ifndef _ALEPH_PRGM_SCALE_H_
#define _ALEPH_PRGM_SCALE_H_

#include "types.h"

#define SCALE_NOTE_SIZE 1536
#define SCALE_RATIO_SIZE 144

//init scale
extern void scale_init(void);

//lookup and return a scaled note
//extern f32 note_lookup(s32 val);

//lookup and return a transposition ratio
extern f32 transpose_lookup(s32 ratio);

#endif