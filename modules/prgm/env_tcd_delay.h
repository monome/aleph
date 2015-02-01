//  env_tcd_delay.h
//  aleph

#ifndef aleph_env_tcd_delay_h
#define aleph_env_tcd_delay_h

#include "env_tcd_buffer.h"
#include "types.h"

typedef struct _delay {
    sampleBuffer buffer;

    bufferHead play;
    bufferHead rec;
    
    u8 sync;

    fract32 preLevel;

} delay;


//delay init at pre-allocated memory
extern void delay_init(delay *dly, volatile fract32 *data, u32 frames);

//next
extern fract32 delay_next(delay *dly, fract32 input);

//set loop end point in samples
extern void delay_set_loop(delay *dly, u32 loop);

//set delay time in samples
extern void delay_set_delay(delay *dly, u32 time);

//set play position
extern void delay_set_pos_play(delay *dly, u32 smp);

//set rec position
extern void delay_set_pos_rec(delay *dly, u32 smp);

//set play flag
extern void delay_set_playflag(delay *dly, u8 flag);

//set rec flag
extern void delayFadeN_set_recflag(delay *dly, u8 flag);

//set erase level
extern void delayFadeN_set_pre(delayFadeN* dl, fract32 pre);

//set write level
extern void delayFadeN_set_write(delayFadeN* dl, u8 write);

#endif
