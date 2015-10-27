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

#define SQ_LEN 128                      //sequence length

#define N_TRACKS 8                      //number of tracks
#define N_MODES 8                       //number of track modes

#define N_INPUTS 21                     //number of inputs
#define N_PHYSICAL_INPUTS 3             //number of physical inputs
#define N_DIROUTS 10                    //number of selectable direct outputs
#define LOOP_MIN 32                     //minimum loop
#define DEFAULT_LEVEL 0x3fffffff

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
#define MUTE 0
#define MIX 1
#define TRACK 1
#define SOLO 2
#define PREPARE 3
#define PREPAREMUTED 4

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
char renderEnvelope[16];

//master track parameters
char renderGrp1Level[16];
char renderGrp2Level[16];
char renderMasterLevel[16];

//sequenced parameters
char renderEditPos[16];

char renderTrig[N_TRACKS][16];
char renderTime[N_TRACKS][16];


//sample edit parameters
char renderSamplePos[16];
char renderSampleOffsetA[16];
char renderSampleOffsetB[16];
char renderOffsetCut[16];
char renderLoopCut[16];
char renderSampleLoop[16];


//track
typedef struct _prgmTrack *prgmTrackptr;

typedef struct _prgmTrack {
    //track parameters
    s32 input;                          //input
    s32 inL;                            //input level
    u8 auxsw;                           //aux pre | post
    s32 aux;                            //aux send level
    u8 mixsw;                           //mix | group
    s32 mix;                            //mix level

    s32 m;                              //mode
    s32 env;                            //envelope
    u8 motor;                           //motor on/off
    u8 motordir;                        //motor direction
    u8 mutemix;                         //mute mix send
    u8 muteaux;                         //mute aux send
    u8 mutetrk;                         //mute track output
    u8 solotrk;                         //solo track
    u8 cue;                             //cue
    s32 msr;                            //track measure
    s32 len;                            //track length
    
    u32 uP;                             //unsigned parameter
    s32 sP;                             //signed parameter
    
    //sequenced parameters
    s32 s[SQ_LEN];                      //sample
    s32 e[SQ_LEN];                      //envelope trig
    s32 outL[SQ_LEN];                   //output level
    
} prgmTrack;

prgmTrack *track[N_TRACKS];


//clone
typedef struct _prgmClone *prgmCloneptr;

typedef struct _prgmClone {
    s32 c1[SQ_LEN];                     //column 1 data
    s32 c2[SQ_LEN];                     //column 2 data
} prgmClone;

prgmClone *clone;


typedef struct _masterTrack *masterTrackptr;

typedef struct _masterTrack {
    s32 out3;
    s32 out4;
    s32 external;                       //external input level
    
    s32 grp1;                           //group A level
    s32 grp2;                           //group B level
    s32 output;                         //master output level
    
    //global parameters
    u32 editpos;                        //current edit position
    u8 samplepos;                       //current buffer position
    u8 solo;                            //true if track solo is active
    u8 prepare;                         //true if track mute prepare is active
    
    u32 bpm;                            //CLK mode bpm
    
} masterTrack;

masterTrack *master;


//external function declarations
extern void tracker_init(void);
extern void sq_init(void);

#endif
