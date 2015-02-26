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

#define N_TRACKS 4                      //number of tracks
#define SQ_LEN 32                       //sequencer length


//  sequencer track
typedef struct _sqTrack {
    s32 sqtg[SQ_LEN];
    s32 sqf[SQ_LEN];
    s32 sqc[SQ_LEN];
    s32 sqt[SQ_LEN];
    s32 sqp[SQ_LEN];
    s32 sqlp[SQ_LEN];
    s32 sqfq[SQ_LEN];
    
} sqTrack;

typedef struct _sqTrack *sqTrackptr;

//  track
typedef struct _prgmTrack {
    //output process frame
    u8 flag;
    fract32 (*process)(void *);         //pointer to output process frame
    
    //curve
    env_tcd envAmp;
    
    //parameters process frame
    fix16 pF;                           //frequency
    fract32 pM;                         //mix
    fract32 pL;                         //level
    u32 pP;                             //position | offset
    u32 pLP;                            //loop point
    
    filter_1p_lo pSlew;                 //parameter slew
    
} prgmTrack;

typedef struct _prgmTrack *PrgmTrackptr;

#endif
