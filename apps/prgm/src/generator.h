#ifndef _ALEPH_PRGM_SCALE_H_
#define _ALEPH_PRGM_SCALE_H_

#include "types.h"
#include "fix.h"

//aleph-avr32
#include "memory.h"

#include "util.h"


#define N_GEN 32                        //number of generators
#define N_ENV 10                        //number of envelope shapes
#define N_DST 8                         //number of gen destinations

#define WAVE_SIZE 1024                  //wavetable size
#define WAVE_IPS_NORM 0xae3c
#define WAVE_IDX_SHIFT 21
#define WAVE_IDX_MASK (1 << (WAVE_IDX_SHIFT)) - 1
#define WAVE_IDX_MUL_SHIFT (WAVE_IDX_SHIFT) - 15

#define N_GEN_1 (N_GEN - 1)
#define N_ENV_1 (N_ENV - 1)
#define N_DST_1 (N_DST - 1)
#define WAVE_SIZE_1 (WAVE_SIZE - 1)

//env parameters
char renderEnvPhase[16];
char renderEnvFreq[16];
char renderEnvRate[16];
char renderEnvAtten[16];

//gen parameters
char renderTrackPos[16];
char renderGenPos[16];
char renderGenDens[16];
char renderGenDur[16];

//generator states
u8 trackpos;                            //current track
u8 genpos;                              //current generator
u8 dstpos;                              //current destination


//env
typedef const fract32 (*wave_data) [WAVE_SIZE];
typedef struct _genEnv *genEnvptr;
typedef struct _genEnv {
    fract32 output;                     //output
    wave_data data;                     //pointer to waveform data
    fix16 inc;                          //index increment
    fract32 wave;                       //waveform

    fract32 phase;                      //phase
    fract32 trip;                       //trip
    fract32 *tripPoint;                 //pointer to phase

    fix16 f;                            //frequency
    
    u32 sr;                             //sample rate

    fract32 att;                        //output attenuation
    
    Slew32 incSlew;
} genEnv;


//gen
typedef struct _prgmGen *prgmGenptr;
typedef struct _prgmGen {
    genEnv env[N_GEN];                  //envelope
    s32 gD[N_GEN];                      //destination

    s32 gDen[N_GEN];                    //step density
    s32 gDur[N_GEN];                    //step duration
    
} prgmGen;

prgmGen *gen;


//external function declarations
extern void gen_init(void);
extern void env_set_trig(u8 trig);
extern void env_set_f(genEnv *e, fix16 f);
extern fract32 env_next(genEnv *e);

#endif
