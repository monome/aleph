//standard library
#include <string.h>

//print debug
#include "print_funcs.h"

//aleph-avr32
#include "memory.h"
#include "scale.h"

//static variables
static f32 *clockspeedBuf;
static f32 *straightBuf;
//static f32 *dottedBuf;
//static f32 *tripletsBuf;
//static f32 *shuffledBuf;

static f32 clockspeed[16] = { 90, 102, 108, 111, 114, 117, 120, 123, 126, 129, 132, 138, 141, 144, 150, 180 };
static f32 straight[7] = { 0, 1/32, 1/16, 1/8, 1/4, 1/2, 1 };
//static f32 dotted[11] = { 0, 1/32, dot, 1/16, dot, 1/8, dot, 1/4, dot, 1/2, 1 };
//static f32 triplets[11] = { 0, 1/32, dot, 1/16, dot, 1/8, dot, 1/4, dot, 1/2, 1 };
//static f32 shuffled[17] = { 0, sshort, 1/32, slong, sshort, 1/16, slong, sshort, 1/8, slong, sshort, 1/4, slong, sshort, 1/2, slong, 1 };

//static f32 *justBuf;
//static f32 just[SCALE_JUST_SIZE] = {
//#include "scale_just.c"
//};

//external function definitions
void scale_init (void) {
//    justBuf = (f32*)alloc_mem(SCALE_JUST_SIZE * (sizeof(f32)));
//    justBuf = &just[0];
    
    clockspeedBuf = (f32*)alloc_mem(16 * (sizeof(f32)));
    clockspeedBuf = &clockspeed[0];
    
    straightBuf = (f32*)alloc_mem(7 * (sizeof(f32)));
    straightBuf = &straight[0];
/*
    dottedBuf = (f32*)alloc_mem(10 * (sizeof(f32)));
    dottedBuf = &dotted[0];

    tripletsBuf = (f32*)alloc_mem(10 * (sizeof(f32)));
    tripletsBuf = &triplets[0];

    shuffledBuf = (f32*)alloc_mem(16 * (sizeof(f32)));
    shuffledBuf = &shuffled[0];
*/
}


f32 just_lookup(s32 val) {
//    return just[val];
    return clockspeed[val]; //FOR TESTING PURPOSE!!
}

f32 clockspeed_lookup(s32 val) {
    return clockspeed[val];
}

f32 straight_lookup(s32 val) {
    return straight[val];
}