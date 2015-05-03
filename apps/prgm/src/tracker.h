//
//  tracker.h
//  aleph
//
//  Created by Staffan Jansson on 25/10/14.
//

#ifndef aleph_tracker_h
#define aleph_tracker_h

//mode led ctrl
#include "gpio.h"
#include "aleph_board.h"

//memory allocation
#include "memory.h"

//debug
#include "print_funcs.h"

//spi
#include "bfin.h"

//adc polling rate
//#include "app_timers.h"

//files
#include "files.h"

//#include "generator.h"
#include "types.h"
#include "control.h"
#include "ctl.h"
#include "pages.h"
#include "render.h"

#define SQ_LEN 128                       //sequence length

#define N_TRACKS 8                      //number of tracks
#define N_MODES 6                       //number of track modes

#define N_INPUTS 26                     //number of inputs
#define N_PHYSICAL_INPUTS 5             //number of physical inputs
#define N_DIROUTS 11                    //number of selectable direct outputs
#define LOOP_MIN 32                     //minimum loop
#define DEFAULT_LEVEL 0x2fffffff

#define N_TRACKS_1 (N_TRACKS - 1)
#define N_MODES_1 (N_MODES - 1)
#define N_INPUTS_1 (N_INPUTS - 1)
#define N_PHYSICAL_INPUTS_1 (N_PHYSICAL_INPUTS - 1)
#define N_DIROUTS_1 (N_DIROUTS - 1)
#define N_BUFFERS_1 (N_BUFFERS - 1)
#define N_SAMPLES_1 (N_SAMPLES - 1)
#define N_OFFSETS_1 (N_OFFSETS - 1)
#define REC_SIZE_1 (REC_SIZE - 1)
#define AUX_SIZE_1 (AUX_SIZE - 1)

#define DUMMY 0                         //dummy step for global parameters

//sequencer parameters
u32 editpos;                            //current edit position
char renderEditPos[16];

//track parameters
char renderInputLevel[N_TRACKS][16];
char renderAux1Level[N_TRACKS][16];
char renderAux2Level[N_TRACKS][16];
char renderLength[N_TRACKS][16];
char renderMixPan[N_TRACKS][16];
char renderMixLevel[N_TRACKS][16];
char renderTrackLevel[16];
char renderParam1[N_TRACKS][16];
char renderParam2[N_TRACKS][16];

//master track parameters
char renderAux1Pan[16];
char renderAux2Pan[16];
char renderMasterLevel[16];

//sequenced parameters
char renderTrig[N_TRACKS][16];
char renderTime[N_TRACKS][16];
char renderEnvDur[N_TRACKS][16];
char renderEnvLevel[N_TRACKS][16];

//track
typedef struct _prgmTrack *prgmTrackptr;

typedef struct _prgmTrack {
    //track parameters
    s32 input;                          //input
    s32 inL;                            //input level
    s32 aux1;                           //aux1 level
    s32 aux2;                           //aux2 level
    s32 pan;                            //mix pan
    s32 mix;                            //mix level

    s32 m;                              //mode
    u8 mutemix;                         //mute mix
    u8 mutetrk;                         //mute track
    s32 len;                            //track length
    s32 msr;                            //track measure
    
    u32 uP;                             //unsigned parameter
    s32 sP;                             //signed parameter
    
    //sequenced parameters
    s32 s[SQ_LEN];                      //sample
    s32 outL[SQ_LEN];                   //output level
    
} prgmTrack;

prgmTrack *track[N_TRACKS];


typedef struct _masterTrack *masterTrackptr;

typedef struct _masterTrack {
    s32 pan1;                           //aux1 pan
    s32 pan2;                           //aux2 pan
    
    s32 out3;                           //direct output 3
    s32 out4;                           //direct output 4

    s32 output;                         //master output level
    
} masterTrack;

masterTrack *master;


//external function declarations
extern void tracker_init(void);
extern void sq_init(void);

#endif
