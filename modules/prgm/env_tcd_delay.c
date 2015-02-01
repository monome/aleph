//  env_tcd_delay.c
//  aleph

#include "env_tcd_delay.h"


void delay_init(delay *dly, volatile fract32 *data, u32 frames) {
    buffer_init(&(dly->buffer), data, frames);
    
    buffer_head_init(&(dly->play), &(dly->buffer));
    buffer_head_init(&(dly->rec), &(dly->buffer));
    
    dly->play.idx = 0;
    dly->play.loop = frames;
    
    dly->rec.idx = 0;
    dly->rec.loop = frames;
    
    dly->preLevel = 0;
    dl->flag = 1;
}


fract32 delay_next(delay *dly, fract32 input) {
    fract32 tmp;
    
    //  play
    tmp = buffer_head_play(&(dly->play));
    
    if(dly->preLevel == 0)
    {
        //  write and replace
        buffer_head_record(&(dly->rec), input);
    }
    else if(dly->preLevel < 0)
    {
        //  overdub
        buffer_head_dub(&(dly->rec), input);
    }
    else
    {
        //  record mix
        buffer_head_mix(&(dly->rec), input, dly->preLevel);
    }
    
    //advance the phasors
    buffer_head_next(&(dly->play));
    buffer_head_next(&(dly->rec));
    
    return tmp;
}


extern void delay_set_loop(delay *dly, u32 loop) {
    dly->play.loop = loop - 1;
    dly->rec.loop = loop - 1;
}


extern void delay_set_delay(delay *dly, u32 time) {
    buffer_head_sync(&(dly->play), &(dly->rec), time);
}


extern void delay_set_preLevel(delay *dly, fract32 pre) {
    dly->preLevel = pre;
}
