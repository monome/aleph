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

//other stuff
#include "types.h"
#include "control.h"
#include "ctl.h"
#include "pages.h"
#include "render.h"
#include "scale.h"

#define SQ_LEN 16                           //sequence length
#define N_SQ 4                              //number of tracks
#define N_MODES 11                          //number of modes
#define N_MODES_1 (N_MODES - 1)
#define BUF_SIZE 0xBB800
#define BUF_SIZE_1 (BUF_SIZE -1)

s32 modetmp0;
s32 modetmp1;
s32 modetmp2;
s32 modetmp3;

char renderTrig0[16];
char renderTrig1[16];
char renderTrig2[16];
char renderTrig3[16];

char renderTime0[16];
char renderTime1[16];
char renderTime2[16];
char renderTime3[16];

char renderL0[16];
char renderL1[16];
char renderL2[16];
char renderL3[16];

char renderP0[16];
char renderP1[16];
char renderP2[16];
char renderP3[16];

char renderF0[16];
char renderF1[16];
char renderF2[16];
char renderF3[16];

char renderCounter[16];

u8 counter;
u8 length;

typedef struct _modeflag {
    u8 have_time : 1;
    u8 have_pL : 1;
    u8 have_pP : 1;
    u8 have_pF : 1;
    u8 pF_have_scale : 1;
    u8 have_pX : 1;
} modeflag;

typedef struct _prgmTrack *prgmTrackptr;

typedef struct _prgmTrack {
    s32 trig[SQ_LEN];

    modeflag *m[SQ_LEN];
    s32 modename[SQ_LEN];
    s32 f[SQ_LEN];
    s32 c[SQ_LEN];
    s32 t[SQ_LEN];
    
    s32 pL[SQ_LEN];                     //level
    s32 pP[SQ_LEN];                     //position | phase | filepos | mempos
    s32 pF[SQ_LEN];                     //frequency
    s32 pFs[SQ_LEN];                    //scaled frequency
    s32 pX[SQ_LEN];                     //q | slew | offset | blend
} prgmTrack;

prgmTrack *track[N_SQ];

extern void tracker_init(void);
extern void set_mode(prgmTrack *track, s32 m);
extern u8 get_mode(prgmTrack *track, u8 i);
extern void play(s32 trig);
extern void return_to_one(s32 trig);
extern void play_step(s32 trig);

#endif