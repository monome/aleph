//prgm.c
//aleph-prgm-avr32

#include "page_env.h"

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

static s32 knob_accel(s32 inc);

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

s32 knob_accel(s32 inc) {
    s32 incAbs = inc < 0 ? inc * -1 : inc; //if inc < 0 incAbs = (inc * -1), else incAbs = inc
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
    
    if(state_sw ==3) play_step(1);
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
            print_fix16(renderTrig0, prgmtrack[i]->tg0);
            print_fix16(renderTrig1, prgmtrack[i]->tg1);
            print_fix16(renderTrig2, prgmtrack[i]->tg2);
            print_fix16(renderTrig3, prgmtrack[i]->tg3);

            print_fix16(renderTime0, prgmtrack[i]->ct0);
            print_fix16(renderTime1, prgmtrack[i]->ct1);
            print_fix16(renderTime2, prgmtrack[i]->ct2);
            print_fix16(renderTime3, prgmtrack[i]->ct3);
            
            print_fix16(renderCounter, (i + 1) * 0x00010000);

            render_curve(i);
            
            render_trig();
            render_time();
            render_countenv();
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
        
        render_level();
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
                tmp = prgmtrack[i]->tg0;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                prgmtrack[i]->tg0 = tmp;
                print_fix16(renderTrig0, tmp);
                render_trig();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[i]->c0;
                tmp += val;
                //  cycle values
                if (tmp < 0) tmp = N_CURVES_1;
                if (tmp > N_CURVES_1) tmp = 0;
                prgmtrack[i]->c0 = tmp;
//                set_playmode(tmp);
                ctl_param_change(eParamCurve0, tmp);
                render_curvename(0, tmp);
                render_trig();
                render_time();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = prgmtrack[i]->ct0;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->ct0 = tmp;
                ctl_param_change(eParamCurveTime0, tmp);
                print_fix16(renderTime0, tmp);
                render_time();
            }
            break;

        case 3:
            break;

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
                tmp = prgmtrack[i]->tg1;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[i]->tg1 = tmp;
                print_fix16(renderTrig1, tmp);
                render_trig();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->c1;
                tmp += val;
                if (tmp < 0) tmp = N_CURVES_1;
                if (tmp > N_CURVES_1) tmp = 0;
                prgmtrack[i]->c1 = tmp;
                ctl_param_change(eParamCurve1, tmp);
                render_curvename(1, tmp);
                render_trig();
                render_time();
            }
            break;

        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = prgmtrack[i]->ct1;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->ct1 = tmp;
                ctl_param_change(eParamCurveTime1, tmp);
                print_fix16(renderTime1, tmp);
                render_time();
            }
            break;
            
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
                tmp = prgmtrack[i]->tg2;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[i]->tg2 = tmp;
                print_fix16(renderTrig2, tmp);
                render_trig();
            }
            break;

        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[i]->c2;
                tmp += val;
                if (tmp < 0) tmp = N_CURVES_1;
                if (tmp > N_CURVES_1) tmp = 0;
                prgmtrack[i]->c2 = tmp;
                ctl_param_change(eParamCurve2, tmp);
                render_curvename(2, tmp);
                render_trig();
                render_time();
            }
            break;

        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = prgmtrack[i]->ct2;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->ct2 = tmp;
                ctl_param_change(eParamCurveTime2, tmp);
                print_fix16(renderTime2, tmp);
                render_time();
            }
            break;
            
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
                tmp = prgmtrack[i]->tg3;
                tmp += val;
                if (tmp <= 0) tmp = 0;
                if (tmp >= 1) tmp = 1;
                prgmtrack[i]->tg3 = tmp;
                print_fix16(renderTrig3, tmp);
                render_trig();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[i]->c3;
                tmp += val;
                if (tmp < 0) tmp = N_CURVES_1;
                if (tmp > N_CURVES_1) tmp = 0;
                prgmtrack[i]->c3 = tmp;
                ctl_param_change(eParamCurve3, tmp);
                render_curvename(3, tmp);
                render_trig();
                render_time();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = prgmtrack[i]->ct3;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                prgmtrack[i]->ct3 = tmp;
                ctl_param_change(eParamCurveTime3, tmp);
                print_fix16(renderTime3, tmp);
                render_time();
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
