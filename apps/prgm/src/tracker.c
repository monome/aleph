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

modeflag off = { .have_time = 0, .have_pL = 0, .have_pP = 0, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };
modeflag HOLD = { .have_time = 0, .have_pL = 0, .have_pP = 0, .have_pF = 1, .pF_have_scale = 0, .have_pX= 0 };
//modeflag justHOLD = { .have_time = 0, .have_pL = 0, .have_pP = 0, .have_pF = 1, .pF_have_scale = 1, .have_pX= 0 };
//modeflag slewHOLD = { .have_time = 0, .have_pL = 0, .have_pP = 0, .have_pF = 1, .pF_have_scale = 1, .have_pX= 0 };
modeflag TRIG = { .have_time = 0, .have_pL = 0, .have_pP = 0, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };
modeflag GATE = { .have_time = 1, .have_pL = 0, .have_pP = 0, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };
modeflag NOISE = { .have_time = 1, .have_pL = 0, .have_pP = 0, .have_pF = 0, .pF_have_scale = 0, .have_pX = 1 };
modeflag one = { .have_time = 1, .have_pL = 0, .have_pP = 1, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };
modeflag loop = { .have_time = 1, .have_pL = 0, .have_pP = 1, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };
modeflag scrub = { .have_time = 1, .have_pL = 0, .have_pP = 1, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };
modeflag noise = { .have_time = 1, .have_pL = 0, .have_pP = 0, .have_pF = 0, .pF_have_scale = 0, .have_pX = 1 };
modeflag recin0 = { .have_time = 1, .have_pL = 0, .have_pP = 0, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };
modeflag TGrecin1 = { .have_time = 1, .have_pL = 0, .have_pP = 0, .have_pF = 0, .pF_have_scale = 0, .have_pX = 0 };

prgmTrackptr alloc_track(void) {
    return(prgmTrackptr)alloc_mem(sizeof(prgmTrack));
}

void track_init_param(prgmTrack *t) {
    u8 i;
    for(i=0;i<SQ_LEN;i++)
    {
        t->trig[i] = 0;                 //trig | eParamTrig
        t->m[i] = 0;                    //mode
        t->modename[i] = 0;             //modename
        t->f[i] = 0;                    //process flag | eParamFlag
        t->c[i] = 0;                    //curve | eParamCurve
        
        //  parameters
        t->t[i] = 0;                    //time
        t->pL[i] = 0;                   //level
        t->pP[i] = 0;                   //position | phase
        t->pF[i] = 0;                   //frequency
        t->pFs[i] = 0;                  //scaled frequency
        t->pX[i] = 0;                   //x
    }
    
    //for mode switch case statement...
    modetmp0 = 0;
    modetmp1 = 0;
    modetmp2 = 0;
    modetmp3 = 0;
}

void tracker_init(void) {
    u8 i;
    
    for (i=0; i<N_SQ; i++)
    {
        track[i] = alloc_track();
        track_init_param(track[i]);
    }

    counter = 0;
    length = SQ_LEN;
}

void set_mode (prgmTrack *track, s32 m) {
    u8 i = counter;
    
    if (m < 0) m = N_MODES_1;
    if (m > N_MODES_1) m = 0;
    
    switch (m) {
        case 0: //off
            track->m[i] = &off;
            track->f[i] = 2;
            track->c[i] = 0;
            track->modename[i] = 0;
            break;
            
        case 1: //HOLD
            track->m[i] = &HOLD;
            track->f[i] = 4;
            track->c[i] = 1;
            track->modename[i] = 1;
            break;
            
        case 2: //TRIG
            track->m[i] = &TRIG;
            track->f[i] = 3;
            track->c[i] = 2;
            track->modename[i] = 2;
            break;

        case 3: //GATE
            track->m[i] = &GATE;
            track->f[i] = 3;
            track->c[i] = 3;
            track->modename[i] = 3;
            break;
            
        case 4: //NOISE
            track->m[i] = &NOISE;
            track->f[i] = 3;
            track->c[i] = 6;
            track->modename[i] = 4;
            break;
            
        case 5: //one
            track->m[i] = &one;
            track->f[i] = 0;
            track->c[i] = 4;
            track->modename[i] = 5;
            break;
            
        case 6: //loop
            track->m[i] = &loop;
            track->f[i] = 0;
            track->c[i] = 5;
            track->modename[i] = 6;
            break;

        case 7: //scrub
            track->m[i] = &scrub;
            track->f[i] = 0;
            track->c[i] = 4;
            track->modename[i] = 7;
            break;
            
        case 8: //noise
            track->m[i] = &noise;
            track->f[i] = 1;
            track->c[i] = 6;
            track->modename[i] = 8;
            break;
            
        case 9: //recin0
            track->m[i] = &recin0;
            track->f[i] = 1;
            track->c[i] = 7;
            track->modename[i] = 9;
            break;
            
        case 10: //tgrecin1
            track->m[i] = &TGrecin1;
            track->f[i] = 3;
            track->c[i] = 8;
            track->modename[i] = 10;
            break;
            
        default:
            break;
    }
}

u8 get_mode(prgmTrack *track, u8 i) {
    if (track->m[i] == &off) return 0;
    else if (track->m[i] == &HOLD) return 1;
    else if (track->m[i] == &TRIG) return 2;
    else if (track->m[i] == &GATE) return 3;
    else if (track->m[i] == &NOISE) return 4;
    else if (track->m[i] == &one) return 5;
    else if (track->m[i] == &loop) return 6;
    else if (track->m[i] == &scrub) return 7;
    else if (track->m[i] == &noise) return 8;
    else if (track->m[i] == &recin0) return 9;
    else if (track->m[i] == &TGrecin1) return 10;
    
    else return 0;
}

void play(s32 trig) {
    if (trig)
    {
//        gpio_clr_gpio_pin(LED_MODE_PIN);
        
        u8 i, n;
        
        //  move counter
        counter++;
        if (counter < length) i = counter;
        else i = counter = 0;
        
        //  send parameters
        for (n = 0; n < N_SQ; n++)
        {
            prgmTrack *trk = track[n];
            
            if (trk->trig[i])
            {
                ctl_param_change(eParamFlag0 + n, trk->f[i]);
                ctl_param_change(eParamCurve0 + n, trk->c[i]);
                
                if (trk->m[i]->have_time) ctl_param_change(eParamTime0 + n, trk->t[i]);
                if (trk->m[i]->have_pL) ctl_param_change(eParamL0 + n, trk->pL[i]);
                if (trk->m[i]->have_pP) ctl_param_change(eParamP0 + n, trk->pP[i]);
                if (trk->m[i]->have_pF)
                {
                    if(trk->m[i]->pF_have_scale) ctl_param_change(eParamF0 + n, trk->pFs[i]);
                    else ctl_param_change(eParamF0 + n, trk->pF[i]);
                }
                if (trk->m[i]->have_pX) ctl_param_change(eParamX0 + n, trk->pX[i]);
                
                ctl_param_change(eParamTrig0 + n, 1);
            }
        }
        
        //  render counters
        print_fix16(renderCounter, (i + 1) * 0x00010000);
        if (pageIdx != ePageEnv) render_countlev();
        else render_countenv();

//        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void return_to_one(s32 trig) {
    if (trig)
    {
//        gpio_clr_gpio_pin(LED_MODE_PIN);
        
        u8 i, n;
        
        //  zero counter
        i = counter = 0;
        
        //  send parameters
        for (n = 0; n < N_SQ; n++)
        {
            prgmTrack *trk = track[n];
            
            if (trk->trig[i])
            {
                ctl_param_change(eParamFlag0 + n, trk->f[i]);
                ctl_param_change(eParamCurve0 + n, trk->c[i]);
                
                if (trk->m[i]->have_time) ctl_param_change(eParamTime0 + n, trk->t[i]);
                if (trk->m[i]->have_pL) ctl_param_change(eParamL0 + n, trk->pL[i]);
                if (trk->m[i]->have_pP) ctl_param_change(eParamP0 + n, trk->pP[i]);
                if (trk->m[i]->have_pF)
                {
                    if(trk->m[i]->pF_have_scale) ctl_param_change(eParamF0 + n, trk->pFs[i]);
                    else ctl_param_change(eParamF0 + n, trk->pF[i]);
                }
                if (trk->m[i]->have_pX) ctl_param_change(eParamX0 + n, trk->pX[i]);
                
                ctl_param_change(eParamTrig0 + n, 1);
            }
        }
        
        //  render counters
        print_fix16(renderCounter, (i + 1) * 0x00010000);
        if (pageIdx != ePageEnv) render_countlev();
        else render_countenv();
        
//        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void play_step(s32 trig) {
    if (trig)
    {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        
        u8 i, n;
        i = counter;

        //  send parameters
        for (n = 0; n < N_SQ; n++)
        {
            prgmTrack *trk = track[n];
            
            if (trk->trig[i])
            {
                ctl_param_change(eParamFlag0 + n, trk->f[i]);
                ctl_param_change(eParamCurve0 + n, trk->c[i]);
                
                if (trk->m[i]->have_time) ctl_param_change(eParamTime0 + n, trk->t[i]);
                if (trk->m[i]->have_pL) ctl_param_change(eParamL0 + n, trk->pL[i]);
                if (trk->m[i]->have_pP) ctl_param_change(eParamP0 + n, trk->pP[i]);
                if (trk->m[i]->have_pF)
                {
                    if(trk->m[i]->pF_have_scale) ctl_param_change(eParamF0 + n, trk->pFs[i]);
                    else ctl_param_change(eParamF0 + n, trk->pF[i]);
                }
                if (trk->m[i]->have_pX) ctl_param_change(eParamX0 + n, trk->pX[i]);
                
                ctl_param_change(eParamTrig0 + n, 1);
            }
        }
        
        //  render counters
        print_fix16(renderCounter, (i + 1) * 0x00010000);
        if (pageIdx != ePageEnv) render_countlev();
        else render_countenv();
        
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}
