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
    t->f0 = 0;
    t->f1 = 0;
    t->f2 = 0;
    t->f3 = 0;

    t->t0 = 0;
    t->t1 = 0;
    t->t2 = 0;
    t->t3 = 0;
    
    t->ct0 = 0;
    t->ct1 = 0;
    t->ct2 = 0;
    t->ct3 = 0;
    
    t->c0 = 0;
    t->c1 = 0;
    t->c2 = 0;
    t->c3 = 0;
    
    t->d0 = 0;
    t->d1 = 0;
    t->d2 = 0;
    t->d3 = 0;
    
    t->tg0 = 0;
    t->tg1 = 0;
    t->tg2 = 0;
    t->tg3 = 0;
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

//play
void play(s32 trig) {
if (trig)
{
    u8 i;
    gpio_clr_gpio_pin(LED_MODE_PIN);

    counter++;
    if (counter < length) i = counter;
    else i = counter = 0;
    
    ctl_param_change(eParamFree0, prgmtrack[i]->f0);
    ctl_param_change(eParamFree1, prgmtrack[i]->f1);
    ctl_param_change(eParamFree2, prgmtrack[i]->f2);
    ctl_param_change(eParamFree3, prgmtrack[i]->f3);
    ctl_param_change(eParamTransposed0, transpose_lookup(prgmtrack[i]->t0));
    ctl_param_change(eParamTransposed1, transpose_lookup(prgmtrack[i]->t1));
    ctl_param_change(eParamTransposed2, transpose_lookup(prgmtrack[i]->t2));
    ctl_param_change(eParamTransposed3, transpose_lookup(prgmtrack[i]->t3));
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
    
    //  render counters
    if (pageIdx != ePageEnv)
    {
        print_fix16(renderCounter, i * 0x00010000);
        render_counters();
    }
    gpio_set_gpio_pin(LED_MODE_PIN);
}
else ;;
}
