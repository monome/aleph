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

//audiolib wavetable
#include "interpolate.h"
#include "table.h"
#include "conversion.h"

//audiolib wavebuffer
//#include "buffer.h"

//audiolib cv-outputs
#include "cv.h"
#include "filter_1p.h"

//bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>
#include "module.h"
#include "params.h"

#define WAVE_SHAPE_NUM 2
#define WAVE_TAB_SIZE 512
#define WAVE_BUF_SIZE 1024
//#define WAVE_BUF_SIZE 0x2bf20

//max characters in wavetable name
#define WAVTAB_LABEL_LEN 8

#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)

#define WAVE_SHAPE_IDX_SHIFT 16
#define WAVE_SHAPE_MASK (1 << (WAVE_SHAPE_IDX_SHIFT)) - 1 //mask to get interpolation multiplier

#define WAVE_IDX_SHIFT 21 // rshift to get table index
#define WAVE_IDX_MUL_SHIFT (WAVE_IDX_SHIFT) - 15 // rshift multiplier to fract16
#define WAVE_IDX_MASK (1 << (WAVE_IDX_SHIFT)) - 1 // mask to get interpolation multiplier

#define WAVE_IPS_NORM 0xae3c //44604, normalized IPS for unipolar 1.31 phase, 48k (0xbb80) sampling rate

//number of oscillators
#define N_OSCILLATORS 4

//number of cv outputs
#define N_CVOUTPUTS 4

//sync trig
#define ON 1
#define OFF 0
s32 synctrig;
u8 state;

typedef fract32 (*wave)[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];
typedef struct _prgmOscillator *PrgmOscillatorpointer;
typedef struct _prgmCvChannel *PrgmCvChannelpointer;

typedef struct _prgmOscillator {
    fract32 frameVal;           //oscillator output
    wave tab;
    fix16 f;                    //scaled
    fix16 t;                    //transposed
    
    fract32 phase;
    fract32 trip;               //trip input
    fract32 *tripPoint;         //pointer to phase

    fix16 inc;                  //index increment
    fract16 wave;              //waveform blend
    
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
