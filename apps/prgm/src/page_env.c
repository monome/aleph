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

static s32 time_knob_accel(s32 inc);

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

s32 time_knob_accel(s32 inc) {
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
    
    if(state_sw == 3) play_step(1);
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
            print_fix16(renderTrig0, track[0]->trig[i]);
            print_fix16(renderTrig1, track[1]->trig[i]);
            print_fix16(renderTrig2, track[2]->trig[i]);
            print_fix16(renderTrig3, track[3]->trig[i]);
            
            print_fix16(renderTime0, track[0]->t[i]);
            print_fix16(renderTime1, track[1]->t[i]);
            print_fix16(renderTime2, track[2]->t[i]);
            print_fix16(renderTime3, track[3]->t[i]);
            
            print_fix16(renderCounter, (i + 1) * 0x00010000);

            render_modes(i);
            
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

        print_fix16(renderL0, track[0]->pL[i]);
        print_fix16(renderL1, track[1]->pL[i]);
        print_fix16(renderL2, track[2]->pL[i]);
        print_fix16(renderL3, track[3]->pL[i]);
        
        print_fix16(renderP0, track[0]->pP[i]);
        print_fix16(renderP1, track[1]->pP[i]);
        print_fix16(renderP2, track[2]->pP[i]);
        print_fix16(renderP3, track[3]->pP[i]);
        
        print_fix16(renderF0, track[0]->pF[i]);
        print_fix16(renderF1, track[1]->pF[i]);
        print_fix16(renderF2, track[2]->pF[i]);
        print_fix16(renderF3, track[3]->pF[i]);
        
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
                tmp = track[0]->trig[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[0]->trig[i] = tmp;
                print_fix16(renderTrig0, tmp);
                render_trig();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = modetmp0;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                modetmp0 = tmp;
                set_mode(track[0], tmp);
                render_mode(0, tmp);
                render_trig();
                render_time();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[0]->t[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > BUF_SIZE_1) tmp = BUF_SIZE_1;
                track[0]->t[i] = tmp;
                ctl_param_change(eParamTime0, tmp);
                print_fix16(renderTime0, tmp);
                render_time();
            }
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
                tmp = track[1]->trig[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[1]->trig[i] = tmp;
                print_fix16(renderTrig1, tmp);
                render_trig();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = modetmp1;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                modetmp1 = tmp;
                set_mode(track[1], tmp);
                render_mode(1, tmp);
                render_trig();
                render_time();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[1]->t[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > BUF_SIZE_1) tmp = BUF_SIZE_1;
                track[1]->t[i] = tmp;
                ctl_param_change(eParamTime1, tmp);
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
                tmp = track[2]->trig[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[2]->trig[i] = tmp;
                print_fix16(renderTrig2, tmp);
                render_trig();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = modetmp2;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                modetmp2 = tmp;
                set_mode(track[2], tmp);
                render_mode(2, tmp);
                render_trig();
                render_time();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = track[2]->t[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > BUF_SIZE_1) tmp = BUF_SIZE_1;
                track[2]->t[i] = tmp;
                ctl_param_change(eParamTime2, tmp);
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
                tmp = track[3]->trig[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[3]->trig[i] = tmp;
                print_fix16(renderTrig3, tmp);
                render_trig();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = modetmp3;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                modetmp3 = tmp;
                set_mode(track[3], tmp);
                render_mode(3, tmp);
                render_trig();
                render_time();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[3]->t[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > BUF_SIZE_1) tmp = BUF_SIZE_1;
                track[3]->t[i] = tmp;
                ctl_param_change(eParamTime3, tmp);
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
