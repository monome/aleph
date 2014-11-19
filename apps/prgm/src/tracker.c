//
//  tracker.c
//  aleph
//
//  Created by Staffan Jansson on 25/10/14.
//
//
//        h->data[SQ_LEN] = (Sq*)alloc_mem(sizeof(Sq));

#include "tracker.h"

//static function declarations
static trackptr alloc_track(void);
static void track_init_param(track *t);

//static function definitions
trackptr alloc_track(void) {
    return(trackptr)alloc_mem(sizeof(track));
}

void track_init_param(track *t) {
//    playmode = 0;
    
    t->tg0 = 0;
    t->tg1 = 0;
    t->tg2 = 0;
    t->tg3 = 0;
    
    t->ct0 = 0;
    t->ct1 = 0;
    t->ct2 = 0;
    t->ct3 = 0;
    
    t->c0 = 0;
    t->c1 = 0;
    t->c2 = 0;
    t->c3 = 0;
    
    t->sf0 = 0;
    t->sf1 = 0;
    t->sf2 = 0;
    t->sf3 = 0;

    t->st0 = 0;
    t->st1 = 0;
    t->st2 = 0;
    t->st3 = 0;

    t->d0 = 0;
    t->d1 = 0;
    t->d2 = 0;
    t->d3 = 0;
    
    t->p0 = 0;
    t->p1 = 0;
    t->p2 = 0;
    t->p3 = 0;
}

//extern function definitions
void tracker_init(void) {
    u8 i;
    
    for (i=0; i<SQ_LEN; i++)
    {
        prgmtrack[i] = alloc_track();
        track_init_param(prgmtrack[i]);
    }

    counter = 0;
    length = SQ_LEN;
}

//return pointer to mode function
void (*get_modeptr(u8 n))() {
    static void (*modes[])() =
    {
        mode_off,
        mode_bypass,
        mode_trig,
        mode_gate,
        mode_play,
        mode_loop,
        mode_lin,
        mode_noise,
        mode_rec0_beta,
        mode_rec1_beta,
    };
    
    return (n < 1 || n > N_MODES) ? *modes[0] : *modes[n];
}

//set mode
void set_playmode(u8 pmode) {
    prgmtrack[counter]->playmode = get_playmodeptr(pmode);
}

//play
void play(s32 trig) {
if (trig)
{
    u8 i;
    gpio_clr_gpio_pin(LED_MODE_PIN);
 
    counter++;
    if (counter < length) i = counter;
    else i = counter = 0;
 
    //send parameters to bfin
    prgmtrack[counter]->playmode(void);
 
    //  render counters
    if (pageIdx != ePageEnv)
    {
        print_fix16(renderCounter, (i + 1) * 0x00010000);
        render_counters();
    }

    gpio_set_gpio_pin(LED_MODE_PIN);
}
    else ;;
}

//modes
//mode: off
void mode_off(void) {
    ctl_param_change(eParamFlag0, prgmtrack[i]->f0);
    ctl_param_change(eParamFlag1, prgmtrack[i]->f1);
    ctl_param_change(eParamFlag2, prgmtrack[i]->f2);
    ctl_param_change(eParamFlag3, prgmtrack[i]->f3);
    
    ctl_param_change(eParamCurve0, prgmtrack[i]->c0);
    ctl_param_change(eParamCurve1, prgmtrack[i]->c1);
    ctl_param_change(eParamCurve2, prgmtrack[i]->c2);
    ctl_param_change(eParamCurve3, prgmtrack[i]->c3);
        
    //  if !trig
    if (!(prgmtrack[i]->tg0));
    //  else if trig && not curve zero || not curve bypass
    else ctl_param_change(eParamCurveTrig0, 1);
    
    if (!(prgmtrack[i]->tg1));
    else ctl_param_change(eParamCurveTrig1, 1);
    
    if (!(prgmtrack[i]->tg2));
    else ctl_param_change(eParamCurveTrig2, 1);
    
    if (!(prgmtrack[i]->tg3));
    else ctl_param_change(eParamCurveTrig3, 1);
}

 
void play(s32 trig) {
if (trig)
{
    u8 i;
    gpio_clr_gpio_pin(LED_MODE_PIN);

    counter++;
    if (counter < length) i = counter;
    else i = counter = 0;
    
    ctl_param_change(eParamFree0, prgmtrack[i]->sf0);
    ctl_param_change(eParamFree1, prgmtrack[i]->sf1);
    ctl_param_change(eParamFree2, prgmtrack[i]->sf2);
    ctl_param_change(eParamFree3, prgmtrack[i]->sf3);
    ctl_param_change(eParamTransposed0, transpose_lookup(prgmtrack[i]->st0));
    ctl_param_change(eParamTransposed1, transpose_lookup(prgmtrack[i]->st1));
    ctl_param_change(eParamTransposed2, transpose_lookup(prgmtrack[i]->st2));
    ctl_param_change(eParamTransposed3, transpose_lookup(prgmtrack[i]->st3));
    ctl_param_change(eParamCurve0, prgmtrack[i]->c0);
    ctl_param_change(eParamCurve1, prgmtrack[i]->c1);
    ctl_param_change(eParamCurve2, prgmtrack[i]->c2);
    ctl_param_change(eParamCurve3, prgmtrack[i]->c3);
    
    //  if !trig
    if (!(prgmtrack[i]->tg0)) ;
    
    //  else if trig && not curve zero || not curve bypass
    else if ((prgmtrack[i]->tg0) && prgmtrack[i]->c0 > 1)
    {
        ctl_param_change(eParamCurveTime0, prgmtrack[i]->ct0);
        ctl_param_change(eParamCurveDest0, prgmtrack[i]->d0);
        ctl_param_change(eParamCurveTrig0, 1);
    }
    
    if (!(prgmtrack[i]->tg1)) ;
    else if ((prgmtrack[i]->tg1) && prgmtrack[i]->c1 > 1)
    {
        ctl_param_change(eParamCurveTime1, prgmtrack[i]->ct1);
        ctl_param_change(eParamCurveDest1, prgmtrack[i]->d1);
        ctl_param_change(eParamCurveTrig1, 1);
    }

    
    if (!(prgmtrack[i]->tg2)) ;
    else if ((prgmtrack[i]->tg2) && prgmtrack[i]->c2 > 1)
    {
        ctl_param_change(eParamCurveTime2, prgmtrack[i]->ct2);
        ctl_param_change(eParamCurveDest2, prgmtrack[i]->d2);
        ctl_param_change(eParamCurveTrig2, 1);
    }

    if (!(prgmtrack[i]->tg3)) ;
    else if ((prgmtrack[i]->tg3) && prgmtrack[i]->c3 > 1)
    {
        ctl_param_change(eParamCurveTime3, prgmtrack[i]->ct3);
        ctl_param_change(eParamCurveDest3, prgmtrack[i]->d3);
        ctl_param_change(eParamCurveTrig3, 1);
    }
    
    print_fix16(renderCounter, (i + 1) * 0x00010000);

    //  render counters
    if (pageIdx != ePageEnv) render_countlev();
    else render_countenv();
    
    gpio_set_gpio_pin(LED_MODE_PIN);
}
else ;;
}

void play_step(s32 trig) {
    if (trig)
    {
        u8 i = counter;
        
        gpio_clr_gpio_pin(LED_MODE_PIN);
        
        ctl_param_change(eParamFree0, prgmtrack[i]->sf0);
        ctl_param_change(eParamFree1, prgmtrack[i]->sf1);
        ctl_param_change(eParamFree2, prgmtrack[i]->sf2);
        ctl_param_change(eParamFree3, prgmtrack[i]->sf3);
        ctl_param_change(eParamTransposed0, transpose_lookup(prgmtrack[i]->st0));
        ctl_param_change(eParamTransposed1, transpose_lookup(prgmtrack[i]->st1));
        ctl_param_change(eParamTransposed2, transpose_lookup(prgmtrack[i]->st2));
        ctl_param_change(eParamTransposed3, transpose_lookup(prgmtrack[i]->st3));
        ctl_param_change(eParamCurve0, prgmtrack[i]->c0);
        ctl_param_change(eParamCurve1, prgmtrack[i]->c1);
        ctl_param_change(eParamCurve2, prgmtrack[i]->c2);
        ctl_param_change(eParamCurve3, prgmtrack[i]->c3);
        
        //  if !trig
        if (!(prgmtrack[i]->tg0)) ;
        
        //  else if trig && not curve zero || not curve bypass
        else if ((prgmtrack[i]->tg0) && prgmtrack[i]->c0 > 1)
        {
            ctl_param_change(eParamCurveTime0, prgmtrack[i]->ct0);
            ctl_param_change(eParamCurveDest0, prgmtrack[i]->d0);
            ctl_param_change(eParamCurveTrig0, 1);
        }
        
        if (!(prgmtrack[i]->tg1)) ;
        else if ((prgmtrack[i]->tg1) && prgmtrack[i]->c1 > 1)
        {
            ctl_param_change(eParamCurveTime1, prgmtrack[i]->ct1);
            ctl_param_change(eParamCurveDest1, prgmtrack[i]->d1);
            ctl_param_change(eParamCurveTrig1, 1);
        }
        
        
        if (!(prgmtrack[i]->tg2)) ;
        else if ((prgmtrack[i]->tg2) && prgmtrack[i]->c2 > 1)
        {
            ctl_param_change(eParamCurveTime2, prgmtrack[i]->ct2);
            ctl_param_change(eParamCurveDest2, prgmtrack[i]->d2);
            ctl_param_change(eParamCurveTrig2, 1);
        }
        
        if (!(prgmtrack[i]->tg3)) ;
        else if ((prgmtrack[i]->tg3) && prgmtrack[i]->c3 > 1)
        {
            ctl_param_change(eParamCurveTime3, prgmtrack[i]->ct3);
            ctl_param_change(eParamCurveDest3, prgmtrack[i]->d3);
            ctl_param_change(eParamCurveTrig3, 1);
        }
        
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
    else ;;
}
