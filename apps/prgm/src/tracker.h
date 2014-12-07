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
#include "app_timers.h"

//other stuff
#include "types.h"
#include "control.h"
#include "ctl.h"
#include "pages.h"
#include "render.h"
#include "scale.h"

#define N_SQ 4                              //number of tracks
#define SQ_LEN 16                           //sequence length
#define N_MODES 11                          //number of modes
#define N_MODES_1 (N_MODES - 1)
#define BUF_SIZE 0xBB800                    //size of the audio buffer
#define BUF_SIZE_1 (BUF_SIZE -1)
#define SCRUB_SIZE 0x1234                   //(time - pos) default in scrub mode

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
    modeflag *m[SQ_LEN];
    s32 modename[SQ_LEN];
    s32 f[SQ_LEN];                      //frame process flag
    s32 c[SQ_LEN];                      //curve
    
    s32 t[SQ_LEN];                      //time
    s32 pL[SQ_LEN];                     //level
    s32 pP[SQ_LEN];                     //position | phase | filepos | mempos
    s32 pF[SQ_LEN];                     //frequency
    s32 pFs[SQ_LEN];                    //scaled frequency
    s32 pX[SQ_LEN];                     //q | slew | offset | blend
} prgmTrack;

prgmTrack *track[N_SQ];

//trig
union _Trig {
    s32 packed;
    u8 track[4];
};

typedef union _Trig Trig;

Trig trigs[SQ_LEN];

extern void adc_init(void);
extern void tracker_init(void);
extern void set_mode(prgmTrack *track, s32 m);
extern u8 get_mode(prgmTrack *track, u8 i);
extern void play(s32 trig);
extern void play_reverse(s32 trig);
extern void return_to_one(s32 trig);
extern void play_step(s32 trig);


//TEST
s32 *testvalptr;

//TEST

////////////////////
/*

for (int k = 0; k < myGrains->size(); k++){
    myGrains->at(k)->nextBuffer(accumBuff,frameSkip, nextFrame,k);


void module_process_frame(void) {
out[cvChn] = track[cvChn]->process(track[cvChn]);


void nextBuf(fract32 *data, u32 frames, u32 offset, u8 i);



void buffer_head_next(bufferHead *head) {
    head->divCount++;
    if(head->divCount >= head->div) {
        head->divCount = 0;
        head->idx += head->inc;
        while(head->idx >= head->loop) {
            head->idx -= head->loop;
        }
    }
}






//
if (menuFlag == false){
    for(int i = 0; i < grainCloud->size(); i++){
        grainCloud->at(i)->nextBuffer(out, numFrames);
    }
}
try {
    theAudio = new MyRtAudio(1,MY_CHANNELS, MY_SRATE, &g_buffSize, MY_FORMAT,true);
} catch (RtError & err) {
    err.printMessage();
    exit(1);
}
try
{
    //open audio stream/assign callback
    theAudio->openStream(&audioCallback);
    //get new buffer size
    g_buffSize = theAudio->getBufferSize();
    //start audio stream
    theAudio->startStream();
    //report latency
    theAudio->reportStreamLatency();
    
 
        
        
        
        
        
        void GrainVoice::nextBuffer(double * accumBuff,unsigned int numFrames,unsigned int bufferOffset, int name)
        {
            


*/

#endif