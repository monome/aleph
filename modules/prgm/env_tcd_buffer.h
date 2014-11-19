/* 
env_tcd_buffer.h
*/

#ifndef _ALEPH_BUFFER_H_
#define _ALEPH_BUFFER_H_

#include "fix.h"
#include "fix32.h"
#include "types.h"

//SDRAM mono audiobuffer
typedef struct _cvBuffer {
    u32 frames;                                 //count of frames
    volatile fract32 *data;                     //pointer to data
} cvBuffer;

//buffer head
typedef struct _bufferHead {
    cvBuffer *buf;                              //pointer to cvbuffer
    u32 loop;                                   //index to loop
    u32 idx;                                    //current index
    u32 inc;                                    //phase increment
    u32 div;                                    //rate divisor
    u32 divCount;                               //current divisor count
} bufferHead;


//extern function definitions
//init audiobuffer at pre-allocated memory
extern void buffer_init(cvBuffer *buf, volatile fract32 *data, u32 frames);

//init head
extern void buffer_head_init(bufferHead *head, cvBuffer *buf);

//set head position
extern void buffer_head_pos(bufferHead *head, u32 samples);

//increment head position
extern void buffer_head_next(bufferHead *head);

//play head (interpolated)
extern fract32 buffer_head_play(bufferHead *head);

//record head (interpolated, overwrite)
extern void buffer_head_rec(bufferHead *head, fract32 val);

#endif
