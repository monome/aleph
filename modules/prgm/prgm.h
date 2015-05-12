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
//#include "cv.h"
#include "filter_1p.h"

//audiolib env
#include "env_tcd.h"
#include "env_tcd_buffer.h"

//bfin
#include "bfin_core.h"
#include "module.h"
#include "params.h"

#define N_TRACKS 8                      //number of tracks
#define SQ_LEN 128                      //sequencer length
#define N_DIROUTS 11                    //number of selectable direct outputs


//  sequencer
typedef struct _prgmSq {
    u8 tg[SQ_LEN];                      //trig
    u32 a[SQ_LEN];                      //offset a
    u32 b[SQ_LEN];                      //offset b
    fract32 l[SQ_LEN];                  //output level
} prgmSq;

typedef struct _prgmSq *PrgmSqptr;


//  track
typedef struct _prgmTrack {
    //output process frame
    u8 flag;
    fract32 (*process)(void *);         //pointer to output process frame
    env_tcd envAmp;                     //dsp process
    fract32 output;                     //track output level
    u32 uP;                             //process u32 parameters
    fract32 sP;                         
    
    //mix parameters
    fract32 mix;                        //mix group level
    fract32 panL;                       //mix group pan
    fract32 panR;    
    fract32 aux1;                       //aux send level
    fract32 aux2;                       
    
    //sequencer parameters    
    u16 len;                            //track length
    u16 c;                              //track counter
    
    filter_1p_lo pSlew;                 //parameter slew
} prgmTrack;

typedef struct _prgmTrack *PrgmTrackptr;


//  mix
typedef struct _prgmMaster {
    fract32 (*output3)(void *);         //pointer to direct output 3
    fract32 (*output4)(void *);         //pointer to direct output 4
        
    fract32 output;                     //master output level
    
    fract32 aux1panL;                   //aux1 pan
    fract32 aux1panR;
    
    fract32 aux2panL;                   //aux2 pan
    fract32 aux2panR;
    
} prgmMaster;

typedef struct _prgmMaster *PrgmMasterptr;


#endif
