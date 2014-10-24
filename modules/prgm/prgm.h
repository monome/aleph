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
#include "env_tcd.h"

//bfin
#include "bfin_core.h"
#include "module.h"
#include "params.h"

//number of cv outputs
#define N_CVOUTPUTS 4

typedef struct _prgmCvChannel *PrgmCvChannelpointer;

typedef struct _prgmCvChannel {
    //curve source
    fix16 f;                            //encoder - free pitch
    filter_1p_lo fSlew;                 //encoder slew for free pitch (smooth response)
    fix16 t;                            //encoder - transposed pitch
    
    //curve
    env_tcd envAmp;
    
    //    env_tcd *first;             //first curve
    //    env_tcd *next;              //next curve
    //    env_tcd *last;              //last curve
} prgmCvChannel;

#endif
