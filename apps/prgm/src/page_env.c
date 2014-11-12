//prgm.c
//aleph-prgm-avr32

// asf
#include "print_funcs.h"

#include "delay.h"
#include "events.h"
#include "event_types.h"

// common
#include "fix.h"

// avr32
#include "bfin.h"
#include "region.h"
#include "app.h"
#include "control.h"
#include "app_event_types.h"

//env
#include "page_env.h"
#include "ctl.h"
#include "render.h"
#include "pages.h"
#include "files.h"
#include "tracker.h"
#include "scale.h"

//static functions
static inline void handle_sw(u8 id, u8 b);
static void handle_switch_0(s32 data);
static void handle_switch_1(s32 data);
static void handle_switch_2(s32 data);
static void handle_switch_3(s32 data);
static void handle_switch_4(s32 data);

static void handle_encoder_0(s32 val);
static void handle_encoder_1(s32 val);
static void handle_encoder_2(s32 val);
static void handle_encoder_3(s32 val);

//handler variables
static etype touched = kNumEventTypes; //total number as defined in event_types.h
static u8 touchedThis = 0;
static u8 state_sw;

static inline u8 check_touch(etype et) {
    if(touched != et) {
        touchedThis = 1;
        touched = et;
    }
    return touchedThis;
}    

void handle_sw(u8 id, u8 b) {
    if(b) {
        state_sw = id;
    }
    else if (!b) {
        state_sw = 0;
    }
}

void handle_switch_0(s32 data) {
    handle_sw(1, data > 0);
}

void handle_switch_1(s32 data) {
    handle_sw(2, data > 0);
}

void handle_switch_2(s32 data) {
    handle_sw(3, data > 0);
}

void handle_switch_3(s32 data) {
    handle_sw(4, data > 0);
    
    if(state_sw == 4)
    {
        u8 i;
        gpio_clr_gpio_pin(LED_MODE_PIN);
        counter++;
        
        if (counter < length) i = counter;
        else i = counter = 0;
        
        if (pageIdx != ePageLevel)
        {
            print_fix16(renderTime0, prgmtrack[i]->ct0);
            print_fix16(renderTime1, prgmtrack[i]->ct1);
            print_fix16(renderTime2, prgmtrack[i]->ct2);
            print_fix16(renderTime3, prgmtrack[i]->ct3);
            
            print_fix16(renderCurve0, prgmtrack[i]->c0);
            print_fix16(renderCurve1, prgmtrack[i]->c1);
            print_fix16(renderCurve2, prgmtrack[i]->c2);
            print_fix16(renderCurve3, prgmtrack[i]->c3);
            
            print_fix16(renderDest0, prgmtrack[i]->d0);
            print_fix16(renderDest1, prgmtrack[i]->d1);
            print_fix16(renderDest2, prgmtrack[i]->d2);
            print_fix16(renderDest3, prgmtrack[i]->d3);
            
            print_fix16(renderTrig0, prgmtrack[i]->tg0);
            print_fix16(renderTrig1, prgmtrack[i]->tg1);
            print_fix16(renderTrig2, prgmtrack[i]->tg2);
            print_fix16(renderTrig3, prgmtrack[i]->tg3);
            
            render_time();
            render_curve();
            render_dest();
            render_trig();
        }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageLevel);
        u8 i = counter;
        
        print_fix16(renderFree0, fix16_mul(prgmtrack[i]->f0, transpose_lookup(prgmtrack[i]->t0)));
        print_fix16(renderFree1, fix16_mul(prgmtrack[i]->f1, transpose_lookup(prgmtrack[i]->t1)));
        print_fix16(renderFree2, fix16_mul(prgmtrack[i]->f2, transpose_lookup(prgmtrack[i]->t2)));
        print_fix16(renderFree3, fix16_mul(prgmtrack[i]->f3, transpose_lookup(prgmtrack[i]->t3)));
        
        print_fix16(renderTransposed0, transpose_lookup(prgmtrack[i]->t0));
        print_fix16(renderTransposed1, transpose_lookup(prgmtrack[i]->t1));
        print_fix16(renderTransposed2, transpose_lookup(prgmtrack[i]->t2));
        print_fix16(renderTransposed3, transpose_lookup(prgmtrack[i]->t3));
        
        print_fix16(renderCounter, (i + 1) * 0x00010000);
        
        render_level();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[counter]->ct0;
                tmp += val * 8;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->ct0 = tmp;
                ctl_param_change(eParamCurveTime0, tmp);
                print_fix16(renderTime0, tmp);
                render_time();
            }
            break;

        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[counter]->c0;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= N_CURVES) tmp = N_CURVES;
                prgmtrack[counter]->c0 = tmp;
                ctl_param_change(eParamCurve0, tmp);
                print_fix16(renderCurve0, tmp);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    tmp = prgmtrack[counter]->d0;
                    tmp += val * 4194304;
                    //  set to next source
                    if (tmp < 0)
                    {
                        if(counter < length)
                            tmp = fix16_mul(prgmtrack[counter + 1]->f0, transpose_lookup(prgmtrack[counter + 1]->t0));
                        else
                            //  if last step, set destination to first step source
                            tmp = fix16_mul(prgmtrack[0]->f0, transpose_lookup(prgmtrack[0]->t0));
                    }
                    prgmtrack[counter]->d0 = tmp;
                    ctl_param_change(eParamCurveDest0, tmp);
                    print_fix16(renderDest0, tmp);
                    render_dest();
                }
            break;
            
        case 3:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    tmp = prgmtrack[counter]->tg0;
                    tmp += val;
                    if (tmp <= 0) tmp = 0;
                    if (tmp >= 1) tmp = 1;
                    prgmtrack[counter]->tg0 = tmp;
                    print_fix16(renderTrig0, tmp);
                    render_trig();
                }
            break;

        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[counter]->ct1;
                tmp += val * 8;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->ct1 = tmp;
                ctl_param_change(eParamCurveTime1, tmp);
                print_fix16(renderTime1, tmp);
                render_time();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[counter]->c1;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= N_CURVES) tmp = N_CURVES;
                prgmtrack[counter]->c1 = tmp;
                ctl_param_change(eParamCurve1, tmp);
                print_fix16(renderCurve1, tmp);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[counter]->d1;
                tmp += val * 4194304;
                //  set to next source
                if (tmp < 0)
                {
                    if(counter < length)
                        tmp = fix16_mul(prgmtrack[counter + 1]->f1, transpose_lookup(prgmtrack[counter + 1]->t1));
                    else
                        //  if last step, set destination to first step source
                        tmp = fix16_mul(prgmtrack[0]->f1, transpose_lookup(prgmtrack[0]->t1));
                }
                prgmtrack[counter]->d1 = tmp;
                ctl_param_change(eParamCurveDest1, tmp);
                print_fix16(renderDest1, tmp);
                render_dest();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[counter]->tg1;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[counter]->tg1 = tmp;
                print_fix16(renderTrig1, tmp);
                render_trig();
            }
            break;
        default:
            break;
    }
}

void handle_encoder_2(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[counter]->ct2;
                tmp += val * 8;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->ct2 = tmp;
                ctl_param_change(eParamCurveTime2, tmp);
                print_fix16(renderTime2, tmp);
                render_time();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[counter]->c2;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= N_CURVES) tmp = N_CURVES;
                prgmtrack[counter]->c2 = tmp;
                ctl_param_change(eParamCurve2, tmp);
                print_fix16(renderCurve2, tmp);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[counter]->d2;
                tmp += val * 4194304;
                //  set to next source
                if (tmp < 0)
                {
                    if(counter < length)
                        tmp = fix16_mul(prgmtrack[counter + 1]->f2, transpose_lookup(prgmtrack[counter + 1]->t2));
                    else
                        //  if last step, set destination to first step source
                        tmp = fix16_mul(prgmtrack[0]->f2, transpose_lookup(prgmtrack[0]->t2));
                }
                prgmtrack[counter]->d2 = tmp;
                ctl_param_change(eParamCurveDest2, tmp);
                print_fix16(renderDest2, tmp);
                render_dest();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[counter]->tg2;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[counter]->tg2 = tmp;
                print_fix16(renderTrig2, tmp);
                render_trig();
            }
            break;
        default:
            break;
    }
}

void handle_encoder_3(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[counter]->ct3;
                tmp += val * 8;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->ct3 = tmp;
                ctl_param_change(eParamCurveTime3, tmp);
                print_fix16(renderTime3, tmp);
                render_time();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[counter]->c3;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= N_CURVES) tmp = N_CURVES;
                prgmtrack[counter]->c3 = tmp;
                ctl_param_change(eParamCurve3, tmp);
                print_fix16(renderCurve3, tmp);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[counter]->d3;
                tmp += val * 4194304;
                //  set to next source
                if (tmp < 0)
                {
                    if(counter < length)
                        tmp = fix16_mul(prgmtrack[counter + 1]->f3, transpose_lookup(prgmtrack[counter + 1]->t3));
                    else
                        //  if last step, set destination to first step source
                        tmp = fix16_mul(prgmtrack[0]->f3, transpose_lookup(prgmtrack[0]->t3));
                }
                prgmtrack[counter]->d3 = tmp;
                ctl_param_change(eParamCurveDest3, tmp);
                print_fix16(renderDest3, tmp);
                render_dest();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[counter]->tg3;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[counter]->tg3 = tmp;
                print_fix16(renderTrig3, tmp);
                render_trig();
            }
            break;
        default:
            break;
    }
}

void select_env(void) {
//assign prgm handlers
    app_event_handlers[ kEventEncoder0 ]	= &handle_encoder_3 ;
    app_event_handlers[ kEventEncoder1 ]	= &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder2 ]	= &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder3 ]	= &handle_encoder_0 ;
    app_event_handlers[ kEventSwitch0 ]     = &handle_switch_0 ;
    app_event_handlers[ kEventSwitch1 ]     = &handle_switch_1 ;
    app_event_handlers[ kEventSwitch2 ]     = &handle_switch_2 ;
    app_event_handlers[ kEventSwitch3 ]     = &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ]     = &handle_switch_4 ;
}
