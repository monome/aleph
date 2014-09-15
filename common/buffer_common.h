
/* buffer_common.h
   aleph-common
 
   declare common buffer data types.
   basically, simple descriptors.

 */

#ifndef _ALEPH_COMMON_BUFFER_H_
#define _ALEPH_COMMON_BUFFER_H_

#include "fix.h"
#include "fix32.h"
#include "types.h"

#define WAVE_SHAPE_NUM 2
#define WAVE_TAB_SIZE 512
#define BUFFER_SIZE 32
#define BUFFER_SIZE_1 31

//buffer descriptor
typedef struct __attribute__((__packed__)) _prgmWaveBuffer {
    //dirty flag
    u8 dirty;
    //buffer position
    u8 bpos;
    //waveshape position
    u8 spos;
    //wavetable position
    s32 tpos;
    //value in fract32
    fract32 wav;
} prgmWaveBuffer;

prgmWaveBuffer *wavBuf;
//ordered set of wavetable values
prgmWaveBuffer wavebuffer[BUFFER_SIZE];

#endif // header guard

/*
 __attribute__((__packed__)) marks a variable or struct element for packing into the smallest space available, omitting any alignment bytes usually added between fields to optimize field accesses.
 */
