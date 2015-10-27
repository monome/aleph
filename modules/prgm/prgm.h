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

//audiolib env
#include "env_tcd.h"
#include "env_tcd_buffer.h"
#include "filter_1p.h"

//bfin
#include "bfin_core.h"
#include "module.h"
#include "params.h"

#define N_TRACKS 8                      //number of tracks
#define SQ_LEN 128                      //sequencer length
#define N_DIROUTS 10                    //number of selectable direct outputs


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

    u8 mute;
    fract32 hold;

    u32 uP;                             //process u32 parameters
    fract32 sP;                         
    
    //mix parameters
    u8 to_mix;                          //route to mix
    u8 to_grp1;                         //route to group 1
    u8 to_grp2;                         //route to group 2
    fract32 mix;                        //mix|group send level
    fract32 aux;                        //aux send level

    //sequencer parameters
    u16 len;                            //track length
    u16 c;                              //track counter
        
    filter_1p_lo pSlew;                 //parameter slew
    filter_1p_lo mSlew;                 //mix slew
} prgmTrack;

typedef struct _prgmTrack *PrgmTrackptr;


//  mix
typedef struct _prgmMaster {
    fract32 (*output3)(void *);         //pointer to direct output 3
    fract32 (*output4)(void *);         //pointer to direct output 4
    fract32 direct;                     //direct 3/4 input level

    fract32 grp1;                       //group A to mix level
    fract32 grp2;                       //group B to mix level
    fract32 output;                     //master output level
} prgmMaster;

typedef struct _prgmMaster *PrgmMasterptr;


#endif
