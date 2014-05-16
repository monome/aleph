//  prgm.h
//  aleph-blackfin

#ifndef _ALEPH_PRGM_H_
#define _ALEPH_PRGM_H_

#include "fix.h"
#include "slew.h"


//symbolic constants, macro substitutions
#define WAVE_TAB_NUM 5

#define WAVE_SHAPE_NUM 5
#define WAVE_SHAPE_BITS 3

#define WAVE_TAB_SIZE 1024

#define WAVE_TAB_BITS 10

#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)

#define WAVE_TAB_MAX16 (WAVE_TAB_SIZE * FIX16_ONE - 1)
#define WAVE_TAB_RSHIFT 29
#define WAVE_TAB_MASK 0x1fffffff
#define WAVE_TAB_LSHIFT 2

#define WAVE_SHAPE_IDX_SHIFT 16 - (WAVE_SHAPE_BITS)
#define WAVE_SHAPE_MASK (1 << (WAVE_SHAPE_IDX_SHIFT)) - 1
#define WAVE_SHAPE_MUL_SHIFT 2
#define WAVE_IDX_SHIFT 21
#define WAVE_IDX_MASK (1 << (WAVE_IDX_SHIFT)) - 1
#define WAVE_IDX_MUL_SHIFT (WAVE_IDX_SHIFT) - 15
#define WAVE_IPS_NORM 0xae3c

//sync trig parameters
extern u8 state;
#define STATE ON 1
#define STATE OFF 0

#define N_OSCILLATORS 4

typedef const fract32 (*wavtab_t) [WAVE_SHAPE_NUM][WAVE_TAB_SIZE];

typedef struct _prgmOscillator *PrgmOscillatorpointer;

typedef struct _prgmOscillator {
    fract32 frameVal;           //oscillator output
//    fract32 osc;                //remove?!
//    wavtab_t val;               //remove?!
    fix16 freq;                 //frequency in hz, fract32?!
    fract32 wave;               //waveform
    fix16 ratio;                  //remove...
    fix16 idx;                  //phase as fractional index
    fix16 idxMod;               //pm
    fix16 inc;                  //index increment
    Slew32 incSlew;
    fract32 invAmp;             //inversion amount, use?!
    fix16 invPhase;             //inversion phase, use?!
//    fract32 amp;                //remove?!
//    Slew16 shapeSlew;
} prgmOscillator;

#endif
