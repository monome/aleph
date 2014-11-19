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
//#include "page_level.h"
//#include "page_env.h"
#include "scale.h"

#define SQ_LEN 8                            //sequence length
#define N_SQ 4                              //number of tracks
#define N_MODES 10                         //number of modes (curve -> process frame -> output)
#define N_MODES_1 (N_MODES - 1)

char renderTrig0[16];
char renderTrig1[16];
char renderTrig2[16];
char renderTrig3[16];

char renderS0[16];
char renderS1[16];
char renderS2[16];
char renderS3[16];

char renderD0[16];
char renderD1[16];
char renderD2[16];
char renderD3[16];

char renderP0[16];
char renderP1[16];
char renderP2[16];
char renderP3[16];

char renderTime0[16];
char renderTime1[16];
char renderTime2[16];
char renderTime3[16];

char renderCurve0[16];
char renderCurve1[16];
char renderCurve2[16];
char renderCurve3[16];

char renderDest0[16];
char renderDest1[16];
char renderDest2[16];
char renderDest3[16];

char renderCounter[16];

u8 counter;
u8 length;

typedef struct _track {
    void (*(*mode)(void *)[]);              //array of pointers to mode
    s32 *flag[];                            //array of pointers to mode flag
    
    s32 *trig[];                            //trig
    s32 *c[];                               //curve
    s32 *t[];                               //time
    s32 *p0[];                              //param0 (0 - FR32_MAX)
    s32 *p1[];                              //param1 (inf - 0)
    s32 *p2[];                              //param2 (0 - 1)
    
    s32 f0;                                 //mode flag
    s32 f1;
    s32 f2;
    s32 f3;

    s32 tg0;                                //trig
    s32 tg1;
    s32 tg2;
    s32 tg3;

    s32 c0;                                 //curve
    s32 c1;
    s32 c2;
    s32 c3;

    s32 ct0;                                //time
    s32 ct1;
    s32 ct2;
    s32 ct3;
    
    s32 sf0;                                //param0
    s32 sf1;
    s32 sf2;
    s32 sf3;
    
    s32 st0;                                //param1 (scaled)
    s32 st1;
    s32 st2;
    s32 st3;
    
    s32 d0;                                 //param1
    s32 d1;
    s32 d2;
    s32 d3;

} track;

typedef struct _track *trackptr;

track *prgmtrack[SQ_LEN];

//extern function declarations
extern void tracker_init(void);
extern void play(s32 trig);
extern void play_step(s32 trig);
//extern void edit(void);

#endif