/* 
env_tcd_buffer.h
*/

#ifndef _ALEPH_BUFFER_H_
#define _ALEPH_BUFFER_H_

#include "fix.h"
#include "fix32.h"
#include "types.h"

#define BUFFER_SIZE 0x01e84800                  //total size of SDRAM sample buffer (0x07ffffff)

//SDRAM mono audiobuffer
typedef struct _sampleBuffer {
    volatile fract32 *data;                     //pointer to data
    u32 samples;                                //count of samples
} sampleBuffer;

//buffer head
typedef struct _bufferHead {
    sampleBuffer *buf;                          //pointer to buffer
    u32 offset;                                 //offset within buffer
    u32 idx;                                    //current idx
    u32 loop;                                   //index to loop
    u32 inc;                                    //phase increment
    u32 div;                                    //rate divisor
    u32 divCount;                               //current divisor count
} bufferHead;


//init buffer at pre-allocated memory
extern void buffer_init(sampleBuffer *buf, volatile fract32 *data, u32 samples);

//init head
extern void buffer_head_init(bufferHead *head, sampleBuffer *buf);

//increment head idx
extern void buffer_head_next(bufferHead *head);

//set head position
extern void buffer_head_pos(bufferHead *head, u32 pos);

//play sample at idx
extern s32 buffer_head_play(bufferHead *head);

//record sample at idx
extern void buffer_head_rec(bufferHead *head, s32 sample);

//overdub
extern void buffer_head_dub(bufferHead *head, fract32 s);

//mix
extern void buffer_head_mix(bufferHead *head, fract32 s, fract32 preLevel);

//mix
extern void buffer_head_invmix(bufferHead *head, fract32 s, fract32 preLevel);

#endif
