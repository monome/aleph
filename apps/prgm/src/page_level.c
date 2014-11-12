//page_level.c
//aleph-prgm-avr32

// asf
#include "gpio.h"
#include "print_funcs.h"

// common
#include "fix.h"

// avr32
#include "region.h"
#include "app.h"
#include "control.h"
#include "memory.h"

//prgm
#include "pages.h"
#include "page_level.h"
#include "render.h"
#include "ctl.h"
#include "scale.h"
#include "tracker.h"

//handler function declarations
static inline void handle_sw(u8 id, u8 b);
static void handle_switch_0(s32 data);
static void handle_switch_1(s32 data);
static void handle_switch_2(s32 data);
static void handle_switch_3(s32 data);
static void handle_switch_4(s32 data);

static inline u8 check_touch(etype et);
static void handle_encoder_0(s32 val);
static void handle_encoder_1(s32 val);
static void handle_encoder_2(s32 val);
static void handle_encoder_3(s32 val);

//handler variables
static etype touched = kNumEventTypes; //total number as defined in ctl.h
static u8 touchedThis = 0;
static u8 state_sw;

//handler functions (static)
u8 check_touch(etype et) {
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
//
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
        
        if (pageIdx != ePageEnv)
        {
            print_fix16(renderFree0, fix16_mul(prgmtrack[i]->f0, transpose_lookup(prgmtrack[i]->t0)));
            print_fix16(renderFree1, fix16_mul(prgmtrack[i]->f1, transpose_lookup(prgmtrack[i]->t1)));
            print_fix16(renderFree2, fix16_mul(prgmtrack[i]->f2, transpose_lookup(prgmtrack[i]->t2)));
            print_fix16(renderFree3, fix16_mul(prgmtrack[i]->f3, transpose_lookup(prgmtrack[i]->t3)));
            
            print_fix16(renderTransposed0, transpose_lookup(prgmtrack[i]->t0));
            print_fix16(renderTransposed1, transpose_lookup(prgmtrack[i]->t1));
            print_fix16(renderTransposed2, transpose_lookup(prgmtrack[i]->t2));
            print_fix16(renderTransposed3, transpose_lookup(prgmtrack[i]->t3));
            
            print_fix16(renderCounter, (i + 1) * 0x00010000);
            
            render_free();
            render_transposed();
            render_counters();
        }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageEnv);
        u8 i = counter;
        
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
        
        render_env();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[counter]->f0;
                tmp += val * 4096;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f0 = tmp;
                ctl_param_change(eParamFree0, tmp);
                print_fix16(renderFree0, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t0)));
                render_free();
                }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[counter]->f0;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f0 = tmp;
                ctl_param_change(eParamFree0, tmp);
                print_fix16(renderFree0, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t0)));
                render_free();
            }
            break;

        case 2:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[counter]->t0;
                tmp += val;
                //check for lower limit (out of range)
                if (tmp < 0) tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[counter]->t0 = tmp;
                ctl_param_change(eParamTransposed0, transpose_lookup(tmp));
                print_fix16(renderFree0, fix16_mul(prgmtrack[counter]->f0, transpose_lookup(tmp)));
                print_fix16(renderTransposed0, transpose_lookup(tmp));
                render_free();
                render_transposed();
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
                tmp = prgmtrack[counter]->f1;
                tmp += val * 4096;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f1 = tmp;
                ctl_param_change(eParamFree1, tmp);
                print_fix16(renderFree1, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t1)));
                render_free();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[counter]->f1;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f1 = tmp;
                ctl_param_change(eParamFree1, tmp);
                print_fix16(renderFree1, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t1)));
                render_free();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[counter]->t1;
                tmp += val;
                //check for lower limit (out of range)
                if (tmp < 0) tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[counter]->t1 = tmp;
                ctl_param_change(eParamTransposed1, transpose_lookup(tmp));
                print_fix16(renderFree1, fix16_mul(prgmtrack[counter]->f1, transpose_lookup(tmp)));
                print_fix16(renderTransposed1, transpose_lookup(tmp));
                render_free();
                render_transposed();
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
                tmp = prgmtrack[counter]->f2;
                tmp += val * 4096;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f2 = tmp;
                ctl_param_change(eParamFree2, tmp);
                print_fix16(renderFree2, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t2)));
                render_free();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[counter]->f2;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f2 = tmp;
                ctl_param_change(eParamFree2, tmp);
                print_fix16(renderFree2, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t2)));
                render_free();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[counter]->t2;
                tmp += val;
                //check for lower limit (out of range)
                if (tmp < 0) tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[counter]->t2 = tmp;
                ctl_param_change(eParamTransposed2, transpose_lookup(tmp));
                print_fix16(renderFree2, fix16_mul(prgmtrack[counter]->f2, transpose_lookup(tmp)));
                print_fix16(renderTransposed2, transpose_lookup(tmp));
                render_free();
                render_transposed();
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
                tmp = prgmtrack[counter]->f3;
                tmp += val * 4096;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f3 = tmp;
                ctl_param_change(eParamFree3, tmp);
                print_fix16(renderFree3, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t3)));
                render_free();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[counter]->f3;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                prgmtrack[counter]->f3 = tmp;
                ctl_param_change(eParamFree3, tmp);
                print_fix16(renderFree3, fix16_mul(tmp, transpose_lookup(prgmtrack[counter]->t3)));
                render_free();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[counter]->t3;
                tmp += val;
                //check for lower limit (out of range)
                if (tmp < 0) tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[counter]->t3 = tmp;
                ctl_param_change(eParamTransposed3, transpose_lookup(tmp));
                print_fix16(renderFree3, fix16_mul(prgmtrack[counter]->f3, transpose_lookup(tmp)));
                print_fix16(renderTransposed3, transpose_lookup(tmp));
                render_free();
                render_transposed();
            }
            break;
            
        default:
            break;
    }
}

void select_level(void) {
//assign tracker handlers
    app_event_handlers[ kEventEncoder0 ] = &handle_encoder_3 ;
    app_event_handlers[ kEventEncoder1 ] = &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder2 ] = &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder3 ] = &handle_encoder_0 ;
    app_event_handlers[ kEventSwitch0 ] = &handle_switch_0 ;
    app_event_handlers[ kEventSwitch1 ] = &handle_switch_1 ;
    app_event_handlers[ kEventSwitch2 ] = &handle_switch_2 ;
    app_event_handlers[ kEventSwitch3 ] = &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ] = &handle_switch_4 ;
}
