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
#include "page_level.h"
#include "page_env.h"
#include "scale.h"

#define SQ_LEN 3                            //sequence length
#define N_SQ 4                              //number of tracks
#define N_CURVES 5                          //number of curves in env_tcd

char renderCounter[16];

u8 counter;
u8 length;

typedef struct _track {
    s32 f0;
    s32 f1;
    s32 f2;
    s32 f3;
    
    s32 t0;
    s32 t1;
    s32 t2;
    s32 t3;
    
    s32 ct0;
    s32 ct1;
    s32 ct2;
    s32 ct3;
    
    s32 c0;
    s32 c1;
    s32 c2;
    s32 c3;
    
    s32 d0;
    s32 d1;
    s32 d2;
    s32 d3;
    
    s32 tg0;
    s32 tg1;
    s32 tg2;
    s32 tg3;
} track;

typedef struct _track *trackptr;

track *prgmtrack[SQ_LEN];

//extern function declarations
extern void tracker_init(void);
extern void edit(void);
extern void play(s32 trig);

#endif