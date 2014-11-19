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
#define N_CVOUTPUTS 4

//buffer size for sample based curves
#define PRGM_BUF_FRAMES 0x2EE00 //4 seconds

typedef struct _prgmCvChannel *PrgmCvChannelpointer;

typedef struct _prgmCvChannel {
    //process frame
    fract32 (*process)(void *);         //pointer to process algorithm
    u8 flag;
    
    //curve
    env_tcd envAmp;

    //curve source
    fix16 f;                            //encoder - free pitch
    filter_1p_lo fSlew;                 //encoder slew for free pitch (smooth response)
    fix16 t;                            //encoder - transposed pitch
} prgmCvChannel;

#endif
