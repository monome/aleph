
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

//buffer descriptor
typedef struct __attribute__((__packed__)) BufferDataStruct {
    //data count
    u32 bufcount;
    //pointer to data
    fract32 *bufdata;
} BufferData;

typedef struct __attribute__((__packed__)) BufferTapStruct {
    //current position
    u32 bufpos;
    //pointer to buffer
    BufferData *buf;
} BufferTap;

//union type for byteswapping
typedef union __attribute__((__packed__)) {
    fract32 asFract32;
    u8 asByte[4];
} FrameSwap;

//fract32 bufdata;

extern void init_buffer(BufferData *buf, fract32 *data, u32 count);
extern void init_buffer_tap(BufferTap *tap, BufferData *buf);
extern void buffer_tap_next(BufferTap *tap);
extern void buffer_tap_set_pos(BufferTap *tap, u32 pos);
extern fract32 buffer_tap_read(BufferTap *tap);
extern void buffer_tap_write(BufferTap *tap, fract32 data);

//extern void init_buffer(BufferData *buf, volatile fract32 *data, u32 frames);
//extern void init_buffer_tap(BufferTap *tap, BufferData *buf);
//extern void buffer_tap_next(BufferTap *tap);
//extern void buffer_tap_set_pos(BufferTap *tap, fix32 samples);
//extern fract32 buffer_tap_read(BufferTap *tap);
//extern void buffer_tap_write(BufferTap *tap, fract32 val);

//typedef fract32 (*WavtabData)[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];

#endif // header guard

/*
 __attribute__((__packed__)) marks a variable or struct element for packing into the smallest space available, omitting any alignment bytes usually added between fields to optimize field accesses.
 */
