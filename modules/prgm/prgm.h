//  prgm.h
//  aleph-blackfin

#ifndef _ALEPH_PRGM_H_
#define _ALEPH_PRGM_H_

//standard libraries
#include <stdlib.h>
#include <string.h>

//aleph-common
#include "fix.h"
#include "types.h"

//audiolib cv-outputs
#include "cv.h"
#include "filter_1p.h"

//audiolib env
#include "env_tcd.h"
#include "env_tcd_buffer.h"

//bfin
#include "bfin_core.h"
#include "module.h"
#include "params.h"

//number of cv outputs
#define N_TRACKS 4
#define SQ_LEN 16

//pattern
typedef u64 Align;

union sq {
    struct {
        union sq *ptr;
        u32 t;                          //time
        u32 pP;                         //position
    } s;
    Align forced;
};

typedef union sq SQ;
typedef union sq *SQptr;

//        u8 trig;                //step trig
//        u8 f;                   //frame process flag
//        u8 c;                   //curve
//        s32 t;                  //time
//        s32 pL;                 //level
//        s32 pF;                 //frequency
//        s32 pX;                 //x

//track
typedef struct _prgmTrack {
    //process frame
    u8 flag;
    fract32 (*process)(void *);         //pointer to process algorithm
    
    //curve
    env_tcd envAmp;

    //parameters
    fract32 pL;                         //level
//    u32 pP;                             //postion | phase | pan
    u32 *pP;                            //pointer to position
    fix16 pF;                           //frequency
    filter_1p_lo pFSlew;                //frequency slew
    fract32 pX;                        //pointer to x
    
} prgmTrack;

typedef struct _prgmTrack *PrgmTrackptr;

#endif
