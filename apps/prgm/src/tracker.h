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

//other stuff
#include "types.h"
#include "control.h"
#include "ctl.h"
#include "pages.h"
#include "render.h"
#include "scale.h"

#define N_TRACKS 4                          //number of tracks
#define SQ_LEN 48                           //sequence length

#define N_MODES 16                          //number of modes
#define N_MODES_1 (N_MODES - 1)

#define N_INPUTS 26                         //number of selectable inputs
#define N_INPUTS_1 (N_INPUTS - 1)

#define N_PHYSICAL_INPUTS 4                 //number of selectable physical inputs
#define N_PHYSICAL_INPUTS_1 (N_INPUTS - 1)

#define N_BUFFERS_1 (N_BUFFERS - 1)
#define REC_SIZE_1 (REC_SIZE - 1)
#define AUX_SIZE_1 (AUX_SIZE - 1)

//#define SCRUB_SIZE 0x1234                   //(time - pos) default in scrub mode
//#define FRAMES 800

#define DUMMY 0                             //dummy step for global parameters

//counters
char renderLength[16];

char renderStepLength[16];
char renderTempo[16];
char renderEditPosition[16];
char renderBufferPosition[16];

//page env
char renderTrig0[16];
char renderTrig1[16];
char renderTrig2[16];
char renderTrig3[16];

char renderTime0[16];
char renderTime1[16];
char renderTime2[16];
char renderTime3[16];

//page mode
char renderPosition0[16];
char renderPosition1[16];
char renderPosition2[16];
char renderPosition3[16];

char renderLoop0[16];
char renderLoop1[16];
char renderLoop2[16];
char renderLoop3[16];

char renderMix0[16];
char renderMix1[16];
char renderMix2[16];
char renderMix3[16];

char renderLevel0[16];
char renderLevel1[16];
char renderLevel2[16];
char renderLevel3[16];

char renderFrequency0[16];
char renderFrequency1[16];
char renderFrequency2[16];
char renderFrequency3[16];

//sequencer states
u16 editpos;                             //edit position
u16 length;                              //current length

u8 n_scale_lookup[SQ_LEN];              //note length position in lookup table
u8 n_scale[SQ_LEN];                     //scaled note length
u16 tempo_lookup;                        //tempo value position in lookup table
u16 tempo;                               //tempo
u16 measure_lookup;                      //measure value position in lookup table
u16 measure;                             //current measure
u8 motor;                               //motor on|off state

//buffer states
u8 bufferpos;

//track
typedef struct _prgmTrack *prgmTrackptr;

typedef struct _prgmTrack {
    //mode
    s32 m[SQ_LEN];                      //mode
    
    //curve
    s32 c[SQ_LEN];                      //curve
    s32 cT[SQ_LEN];                     //curve time
    s32 cTG[SQ_LEN];                    //curve trig state
    
    //frame process
    s32 f[SQ_LEN];                      //frame process flag

    //sequenced parameters
    s32 pS[SQ_LEN];                     //sample
    s32 pP[SQ_LEN];                     //position | offset | phase
    s32 pLP[SQ_LEN];                    //loop point
    
    s32 pF[SQ_LEN];                     //frequency
    s32 pF_scale[SQ_LEN];               //scaled frequency
    s32 pX[SQ_LEN];                     //q | slew | blend | pw

    //global parameters
    s32 inA;                            //input A
    s32 inB;                            //input B
    s32 mix;                            //mix level
    s32 aux;                            //aux level
    
} prgmTrack;

prgmTrack *track[N_TRACKS];


//external function declarations
extern void tracker_init(void);

#endif
