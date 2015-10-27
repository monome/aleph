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
        t->auxsw = 0;
        t->aux = 0;
        t->mixsw = 0;
        t->mix = 0;
        
        t->m = 0;
        t->env = 0;
        t->motor = 0;
        t->motordir = 1;
        t->mutemix = MIX;
        t->mutetrk = TRACK;
        t->solotrk = TRACK;
        t->cue = 0;
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

    master->out3 = 0;
    master->out4 = 0;
    master->external = 0;
    master->grp1 = FR32_MAX;
    master->grp2 = FR32_MAX;
    master->output = FR32_MAX;
    master->bpm = 1; //120 bpm
    
    //  global parameters
    master->editpos = 0;
    master->samplepos = 9;
    master->solo = 0;
    master->prepare = 0;
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
