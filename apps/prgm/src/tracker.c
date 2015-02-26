//
//  tracker.c
//  aleph
//
//  Created by Staffan Jansson on 25/10/14.
//
//
//

#include "tracker.h"


//static function declarations
static prgmTrackptr alloc_track(void);
static void track_init_param(prgmTrack *t);

//allocate one chunk of memory for each track
prgmTrackptr alloc_track(void) {
    return(prgmTrackptr)alloc_mem(sizeof(prgmTrack));
}

//init track parameters
void track_init_param(prgmTrack *t) {
    u8 i;
    
    //  sequenced parameters
    for(i=0;i<SQ_LEN;i++)
    {
        t->m[i] = 0;
        t->c[i] = 0;
        t->cT[i] = 0;
        t->cTG[i] = 0;
        t->f[i] = 0;
        t->pS[i] = 8;
        t->pP[i] = 0;
        t->pLP[i] = 256;
        t->pF[i] = 0;
        t->pF_scale[i] = 0;
        t->pX[i] = 0;
    }
    
    //  global parameters
    t->inA = 0;
    t->inB = 0;
    t->mix = 0;
    t->aux = 0;
}

//init tracker: four track sequencer
void tracker_init(void) {
    u8 i;
    
    for (i=0; i<N_TRACKS; i++)
    {
        track[i] = alloc_track();
        track_init_param(track[i]);
    }
    
    for(i=0;i<SQ_LEN;i++)
    {
        n_scale_lookup[i] = 0;
        n_scale[i] = 0;
    }

//    bfinheadstate = 0;
//    bfinheadpos = 0;
    motor = 0;

    editpos = 16;
    length = SQ_LEN;
    bufferpos = 0;
    
    tempo_lookup = 0;
    tempo = 0;
    measure_lookup = 0;
    measure = 0;

//    foot1_touched = 0;
//    foot2_touched = 0;
}
