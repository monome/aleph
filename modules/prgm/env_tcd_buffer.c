/*
env_tcd_buffer.c
*/

#include "fract_math.h"
#include <fract2float_conv.h>

#include "fix.h"
#include "fix32.h"
#include "module.h"

#include "env_tcd_buffer.h"

//extern function definitions
void buffer_init(inputBuffer *buf, volatile fract32 *data, u32 frames) {
    buf->data = data;
    buf->frames = frames;
}

void buffer_head_init(bufferHead *head, inputBuffer *buf) {
    head->buf = buf;
    head->loop = buf->frames;
    head->inc = 1;
    head->div = 1;
    head->idx = 0;
    head->divCount = 0;
}

void buffer_head_pos(bufferHead *head, u32 samps) {
    while(samps > head->loop) {
        samps -= head->loop;
    }
    head->idx = samps;
}

void buffer_head_next(bufferHead *head) {
    head->divCount++;
    if(head->divCount >= head->div) {
        head->divCount = 0;
        head->idx += head->inc;
        while(head->idx >= head->loop) {
            head->idx -= head->loop;
        }
    }
}

fract32 buffer_head_play(bufferHead *head) {
    return head->buf->data[head->idx];
}

void buffer_head_rec(bufferHead *head, fract32 val) {
    head->buf->data[head->idx] = val;
}
