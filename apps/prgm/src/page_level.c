//page_level.c
//aleph-prgm-avr32

#include "page_level.h"

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

static s32 knob_accel(s32 inc);

//handler variables
static etype touched = kNumEventTypes; //total number of module parameters as defined in ctl.h
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

s32 knob_accel(s32 inc) {
    s32 incAbs = inc < 0 ? inc * -1 : inc;
    if(incAbs == 1) {
        return inc;
    }
    if(incAbs < 6) {
        return inc << 2;
    }
    return inc << 6;
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

//  step +1
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
            print_fix16(renderS0, fix16_mul(prgmtrack[i]->sf0, transpose_lookup(prgmtrack[i]->st0)));
            print_fix16(renderS1, fix16_mul(prgmtrack[i]->sf1, transpose_lookup(prgmtrack[i]->st1)));
            print_fix16(renderS2, fix16_mul(prgmtrack[i]->sf2, transpose_lookup(prgmtrack[i]->st2)));
            print_fix16(renderS3, fix16_mul(prgmtrack[i]->sf3, transpose_lookup(prgmtrack[i]->st3)));
            
            print_fix16(renderD0, prgmtrack[i]->d0);
            print_fix16(renderD1, prgmtrack[i]->d1);
            print_fix16(renderD2, prgmtrack[i]->d2);
            print_fix16(renderD3, prgmtrack[i]->d3);
            
            print_fix16(renderP0, prgmtrack[i]->p0);
            print_fix16(renderP1, prgmtrack[i]->p1);
            print_fix16(renderP2, prgmtrack[i]->p2);
            print_fix16(renderP3, prgmtrack[i]->p3);
            
            print_fix16(renderCounter, (i + 1) * 0x00010000);
            
            render_source();
            render_dest();
            render_param();
            render_countlev();
        }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

//  switch page
void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageEnv);
        u8 i = counter;
        
        print_fix16(renderTrig0, prgmtrack[i]->tg0);
        print_fix16(renderTrig1, prgmtrack[i]->tg1);
        print_fix16(renderTrig2, prgmtrack[i]->tg2);
        print_fix16(renderTrig3, prgmtrack[i]->tg3);
        
        print_fix16(renderTime0, prgmtrack[i]->ct0);
        print_fix16(renderTime1, prgmtrack[i]->ct1);
        print_fix16(renderTime2, prgmtrack[i]->ct2);
        print_fix16(renderTime3, prgmtrack[i]->ct3);
        
        print_fix16(renderCounter, (i + 1) * 0x00010000);
        
        render_env();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    u8 i = counter;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf0;
                //  fine with acceleration
                tmp += knob_accel(val);
                //  check for lower limit
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf0 = tmp;
                ctl_param_change(eParamFree0, tmp);
                print_fix16(renderS0, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st0)));
                render_source();
                }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf0;
                //  coarse
                tmp += val * 4194304;
                //  check for lower limit
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf0 = tmp;
                ctl_param_change(eParamFree0, tmp);
                print_fix16(renderS0, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st0)));
                render_source();
            }
            break;

        case 2:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[i]->st0;
                tmp += val;
                //  check for lower limit (out of range)
                if (tmp < 0) tmp = 0;
                //  check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[i]->st0 = tmp;
                ctl_param_change(eParamTransposed0, transpose_lookup(tmp));
                print_fix16(renderS0, fix16_mul(prgmtrack[i]->sf0, transpose_lookup(tmp)));
                render_source();
            }
            break;

        case 3:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->d0;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[i]->d0 = tmp;
                ctl_param_change(eParamFlag0, tmp);
                print_fix16(renderD0, tmp);
                render_dest();
            }
            break;
/*
        case 3:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[i]->d0;
                tmp += val * 4194304;
                //  set to next source
                if (tmp < 0)
                {
                    if(i < length)
                        tmp = fix16_mul(prgmtrack[i + 1]->sf0, transpose_lookup(prgmtrack[i + 1]->st0));
                    else
                        //  if last step, set destination to first step source
                        tmp = fix16_mul(prgmtrack[0]->sf0, transpose_lookup(prgmtrack[0]->st0));
                }
                prgmtrack[i]->d0 = tmp;
                ctl_param_change(eParamCurveDest0, tmp);
                print_fix16(renderD0, tmp);
                render_dest();
            }
            break;
*/
        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    s32 tmp;
    u8 i = counter;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf1;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf1 = tmp;
                ctl_param_change(eParamFree1, tmp);
                print_fix16(renderS1, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st1)));
                render_source();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf1;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf1 = tmp;
                ctl_param_change(eParamFree1, tmp);
                print_fix16(renderS1, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st1)));
                render_source();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->st1;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[i]->st1 = tmp;
                ctl_param_change(eParamTransposed1, transpose_lookup(tmp));
                print_fix16(renderS1, fix16_mul(prgmtrack[i]->sf1, transpose_lookup(tmp)));
                render_source();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->d1;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[i]->d1 = tmp;
                ctl_param_change(eParamFlag1, tmp);
                print_fix16(renderD1, tmp);
                render_dest();
            }
            break;
/*
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->d1;
                tmp += val * 4194304;
                if (tmp < 0)
                {
                    if(i < length)
                        tmp = fix16_mul(prgmtrack[i + 1]->sf1, transpose_lookup(prgmtrack[i + 1]->st1));
                    else
                        tmp = fix16_mul(prgmtrack[0]->sf1, transpose_lookup(prgmtrack[0]->st1));
                }
                prgmtrack[i]->d1 = tmp;
                ctl_param_change(eParamCurveDest1, tmp);
                print_fix16(renderD1, tmp);
                render_dest();
            }
            break;
*/
        default:
            break;
    }
}

void handle_encoder_2(s32 val) {
    s32 tmp;
    u8 i = counter;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf2;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf2 = tmp;
                ctl_param_change(eParamFree2, tmp);
                print_fix16(renderS2, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st2)));
                render_source();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf2;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf2 = tmp;
                ctl_param_change(eParamFree2, tmp);
                print_fix16(renderS2, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st2)));
                render_source();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[i]->st2;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[i]->st2 = tmp;
                ctl_param_change(eParamTransposed2, transpose_lookup(tmp));
                print_fix16(renderS2, fix16_mul(prgmtrack[i]->sf2, transpose_lookup(tmp)));
                render_source();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->d2;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[i]->d2 = tmp;
                ctl_param_change(eParamFlag2, tmp);
                print_fix16(renderD2, tmp);
                render_dest();
            }
            break;
/*
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[i]->d2;
                tmp += val * 4194304;
                if (tmp < 0)
                {
                    if(i < length)
                        tmp = fix16_mul(prgmtrack[i + 1]->sf2, transpose_lookup(prgmtrack[i + 1]->st2));
                    else
                        tmp = fix16_mul(prgmtrack[0]->sf2, transpose_lookup(prgmtrack[0]->st2));
                }
                prgmtrack[i]->d2 = tmp;
                ctl_param_change(eParamCurveDest2, tmp);
                print_fix16(renderD2, tmp);
                render_dest();
            }
            break;
*/
        default:
            break;
    }
}

void handle_encoder_3(s32 val) {
    s32 tmp;
    u8 i = counter;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf3;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf3 = tmp;
                ctl_param_change(eParamFree3, tmp);
                print_fix16(renderS3, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st3)));
                render_source();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[i]->sf3;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->sf3 = tmp;
                ctl_param_change(eParamFree3, tmp);
                print_fix16(renderS3, fix16_mul(tmp, transpose_lookup(prgmtrack[i]->st3)));
                render_source();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[i]->st3;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                prgmtrack[i]->st3 = tmp;
                ctl_param_change(eParamTransposed3, transpose_lookup(tmp));
                print_fix16(renderS3, fix16_mul(prgmtrack[i]->sf3, transpose_lookup(tmp)));
                render_source();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->d3;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[i]->d3 = tmp;
                ctl_param_change(eParamFlag3, tmp);
                print_fix16(renderD3, tmp);
                render_dest();
            }
            break;
/*
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[i]->d3;
                tmp += val * 4194304;
                if (tmp < 0)
                {
                    if(i < length)
                        tmp = fix16_mul(prgmtrack[i + 1]->sf3, transpose_lookup(prgmtrack[i + 1]->st3));
                    else
                        tmp = fix16_mul(prgmtrack[0]->sf3, transpose_lookup(prgmtrack[0]->st3));
                }
                prgmtrack[i]->d3 = tmp;
                ctl_param_change(eParamCurveDest3, tmp);
                print_fix16(renderD3, tmp);
                render_dest();
            }
            break;
*/
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
