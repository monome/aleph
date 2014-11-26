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

//buffer size for sample based curves
//#define PRGM_BUF_FRAMES 0x2EE00 //4 seconds

typedef struct _prgmTrack *PrgmTrackptr;

typedef struct _prgmTrack {
    //process frame
    u8 flag;
    fract32 (*process)(void *);         //pointer to process algorithm
    
    //curve
    env_tcd envAmp;

    //parameters
    fract32 pL;                         //level
    u32 pP;                             //postion | phase | pan
    fix16 pF;                           //frequency
    filter_1p_lo pFSlew;                //frequency slew
    fract32 pX;                         //x
} prgmTrack;

#endif
