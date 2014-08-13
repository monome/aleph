//  prgm.h
//  aleph-blackfin

#ifndef _ALEPH_PRGM_H_
#define _ALEPH_PRGM_H_

//standard libraries
#include <math.h>
#include <stdlib.h>
#include <string.h>

//aleph-common
#include "fix.h"
#include "slew.h"
#include "types.h"

//audiolib
#include "interpolate.h"
#include "table.h"
#include "conversion.h"

//bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "params.h"
#include "wavtab_common.h"

//cv-outputs
#include "cv.h"
#include "filter_1p.h"

//number of oscillators
#define N_OSCILLATORS 4

//number of cv outputs
#define N_CVOUTPUTS 4

//sync trig
#define ON 1
#define OFF 0
s32 synctrig;
u8 state;

typedef struct _prgmOscillator *PrgmOscillatorpointer;
typedef struct _prgmCvChannel *PrgmCvChannelpointer;

typedef struct _prgmOscillator {
    fract32 frameVal;           //oscillator output
    WavtabData tab;
    fix16 f;                    //scaled
    fix16 t;                    //transposed
    fract32 ffAmount;           //fine amount
    
    fract32 phase;
    fract32 trip;               //trip input
    fract32 *tripPoint;         //pointer to phase

    fix16 inc;                  //index increment
    fract16 wave;               //waveform
    
    Slew32 incSlew;
    Slew16 shapeSlew;
            
    fract32 amp;                
} prgmOscillator;

typedef struct _prgmCvChannel {
    fract32 cvVal;              //cv output
    fix16 f;                    //scaled
    fix16 t;                    //transposed
    filter_1p_lo cvSlew;
} prgmCvChannel;

#endif
