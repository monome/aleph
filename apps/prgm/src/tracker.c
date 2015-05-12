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
static prgmCloneptr alloc_clone(void);
static masterTrackptr alloc_master(void);


//allocate one chunk of memory for each track
prgmTrackptr alloc_track(void) {
    return(prgmTrackptr)alloc_mem(sizeof(prgmTrack));
}

prgmCloneptr alloc_clone(void) {
    return(prgmCloneptr)alloc_mem(sizeof(prgmClone));
}

masterTrackptr alloc_master(void) {
    return(masterTrackptr)alloc_mem(sizeof(masterTrack));
}


//init sequencer values
void sq_init() {
    u8 n;
    u32 i;
    prgmTrack *t;
    
    for(n=0;n<N_TRACKS;n++)
    {
        //init track
        t = track[n];
        
        t->input = 0;
        t->inL = FR32_MAX;
        t->aux1 = 0;
        t->aux2 = 0;
        t->pan = 0;
        t->mix = 0;
        
        t->m = 0;
        t->env = 3;
        t->mutemix = 0;
        t->mutetrk = 0;
        t->len = SQ_LEN;
        t->msr = SQ_LEN / 4;
        
        t->uP = LOOP_MIN;
        t->sP = 0;
        
        for(i=0;i<SQ_LEN;i++)
        {
            t->s[i] = 8;
            t->e[i] = 0;
            t->outL[i] = 0;
        }
    }
    
    for(i=0;i<SQ_LEN;i++)
    {
        clone->c1[i] = 0;
        clone->c2[i] = 0;
    }

    master->output = FR32_MAX;
    master->pan1 = 0;
    master->pan2 = 0;
    master->out3 = 0;
    master->out4 = 0;
        
    //  global parameters
    editpos = 0;
    samplepos = 9;
}

//init tracker: four track sequencer with separate generators
void tracker_init(void) {
    u8 i;
    
    for(i=0;i<N_TRACKS;i++)
    {
        track[i] = alloc_track();
    }
    
    clone = alloc_clone();
    
    master = alloc_master();
    
    sq_init();
}
