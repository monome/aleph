//  prgm.h
//  aleph-blackfin

#ifndef _ALEPH_PRGM_H_
#define _ALEPH_PRGM_H_

#include "fix.h"
#include "slew.h"

//oscillator symbolic constants, macro substitutions
#define WAVE_SHAPE_NUM 5
#define WAVE_SHAPE_BITS 3

#define WAVE_TAB_SIZE 1024
#define WAVE_TAB_BITS 10
#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)

#define WAVE_SHAPE_IDX_SHIFT 16 - (WAVE_SHAPE_BITS)
#define WAVE_SHAPE_MASK (1 << (WAVE_SHAPE_IDX_SHIFT)) - 1
#define WAVE_SHAPE_MUL_SHIFT 2
#define WAVE_IDX_SHIFT 21
#define WAVE_IDX_MASK (1 << (WAVE_IDX_SHIFT)) - 1
#define WAVE_IDX_MUL_SHIFT (WAVE_IDX_SHIFT) - 15
#define WAVE_IPS_NORM 0xae3c

#define N_OSCILLATORS 4

//sync trig variables, symbolic constants
#define ON 1
#define OFF 0
s32 synctrig;
u8 state;

typedef const fract32 (*wavtab_t) [WAVE_SHAPE_NUM][WAVE_TAB_SIZE];

typedef struct _prgmOscillator *PrgmOscillatorpointer;

typedef struct _prgmOscillator {
    fract32 frameVal;           //oscillator output
    wavtab_t tab;
    fix16 f;                    //scale
    fix16 ff;                   //fine
    
    fract32 phase;

    fix16 inc;                  //index increment
    fract16 wave;               //waveform
    
    Slew32 incSlew;
    Slew16 shapeSlew;
            
    fract32 amp;                
} prgmOscillator;

#endif
