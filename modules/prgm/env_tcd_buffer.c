// env_tcd_buffer.c

#include "fract_math.h"
#include <fract2float_conv.h>

#include "fix.h"
#include "fix32.h"
#include "module.h"

#include "env_tcd_buffer.h"

//buffer function definitions
void buffer_init(sampleBuffer *buf, volatile fract32 *data, u32 samples) {
    buf->data = data;
    buf->samples = samples;
}

void buffer_head_init(bufferHead *head, sampleBuffer *buf) {
    head->buf = buf;
    head->idx = 0;
    head->loop = 0;
    head->inc = 1;
    head->div = 1;
    head->divCount = 0;
}

void buffer_head_pos(bufferHead *head, u32 pos) {
    while(pos > head->loop) {
        pos -= head->loop;
    }
    head->idx = pos;
}

void buffer_head_next(bufferHead *head) {
    head->divCount++;
    //    head->idx %= head->loop;
    if(head->divCount >= head->div) {
        head->divCount = 0;
        head->idx += head->inc;
        while(head->idx >= head->loop) {
            head->idx -= head->loop;
        }
    }
}

/*
void buffer_head_prev(bufferHead *head) {
    head->divCount++;
    //    head->idx %= head->loop;
    if(head->divCount >= head->div) {
        head->divCount = 0;
        head->idx += head->inc;
        while(head->idx >= head->loop) {
            head->idx -= head->loop;
        }
    }
}
*/

s32 buffer_head_play(bufferHead *head) {
    return head->buf->data[head->idx];
}

void buffer_head_record(bufferHead *head, s32 sample) {
    head->buf->data[head->idx] = sample;
}
