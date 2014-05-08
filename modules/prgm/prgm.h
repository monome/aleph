//  prgm.h
//  aleph-blackfin

#ifndef _ALEPH_PRGM_H_
#define _ALEPH_PRGM_H_

#include "fix.h"
#include "slew.h"

//symbolic constants, macro substitutions
#define WAVE_TAB_NUM 5
#define WAVE_TAB_SIZE 1024
#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)
#define WAVE_TAB_MAX16 (WAVE_TAB_SIZE * FIX16_ONE - 1)
#define WAVE_TAB_RSHIFT 29
#define WAVE_TAB_MASK 0x1fffffff
#define WAVE_TAB_LSHIFT 2

#define N_OSCILLATORS 4

typedef struct _prgmOscillator {
    fix16 idx;
    fix16 inc;
    fract32 osc;
    fract32 frameVal;
    fix16 freq;
    fract32 wave;
    fract32 amp;
//    filter_1p_lo *freqLp;
//    filter_1p_lo *waveLp;
//    filter_1p_lo *ampLp;    
} prgmOscillator;

extern prgmOscillator *oscillator[N_OSCILLATORS];

#endif
