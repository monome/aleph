/* 
env_tcd_buffer.h
*/

#ifndef _ALEPH_BUFFER_H_
#define _ALEPH_BUFFER_H_

#include "fix.h"
#include "fix32.h"
#include "types.h"

#define BUFFER_SIZE 0x2bf200                    //total size of SDRAM sample buffer
#define N_OFFSETS 32                            //buffer offsets
#define N_RECBUFFERS 4                          //recording buffers
#define RECBUFFER_SIZE 0xBB800                  //16 seconds


//SDRAM mono audiobuffer
typedef struct _sampleBuffer {
    volatile fract32 *data;                     //pointer to data
    u32 samples;                                //count of samples
} sampleBuffer;

//buffer head
typedef struct _bufferHead {
    sampleBuffer *buf;                          //pointer to buffer
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

//play sample at idx
extern s32 buffer_head_play(bufferHead *head);

//record sample at idx
extern void buffer_head_record(bufferHead *head, s32 sample);

#endif
