
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
    u64 bytecount;
    //pointer to data
    volatile u8 *wavbyte;
} BufferData;

typedef struct __attribute__((__packed__)) BufferTapStruct {
    //current position
    u64 bufpos;
    //pointer to buffer
    BufferData *buf;
} BufferTap;

extern void init_buffer(BufferData *buf, volatile u8 *wavbyte, u64 count);
extern void init_buffer_tap(BufferTap *tap, BufferData *buf);
extern void buffer_tap_next(BufferTap *tap);
extern void buffer_tap_set_pos(BufferTap *tap, u64 pos);
extern u8 buffer_tap_read(BufferTap *tap);
extern void buffer_tap_write(BufferTap *tap, u8 wavbyte);

#endif // header guard

/*
 __attribute__((__packed__)) marks a variable or struct element for packing into the smallest space available, omitting any alignment bytes usually added between fields to optimize field accesses.
 */
