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
#include "handler.h"
#include "pages.h"
//#include "util.h" //remove
#include "files.h"

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

//switch3 trig
#define ON 1
#define OFF 0
static u8 switch3;

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
    if(data == 0)
        switch3 = OFF;
    
    else if(switch3 == OFF) {
        switch3 = ON;
        ctl_param_change(eParamCurveTrig0, 1);
        ctl_param_change(eParamCurveTrig0, 0);

        ctl_param_change(eParamCurveTrig1, 1);
        ctl_param_change(eParamCurveTrig1, 0);
        
        ctl_param_change(eParamCurveTrig2, 1);
        ctl_param_change(eParamCurveTrig2, 0);

        ctl_param_change(eParamCurveTrig3, 1);
        ctl_param_change(eParamCurveTrig3, 0);
    }
    
    else if(switch3 == ON)
        ;;
}

void handle_switch_4(s32 data) {
    if(data == 0) { return; }
    
    else {
        set_page(ePageLevel);
        render_level();
    }
}

void handle_encoder_0(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                Time0 += val * 8; //x24 = 4 x 5ms @ 48k
                if (Time0 < 0) Time0 = 0;
                ctl_param_change(eParamCurveTime0, Time0);
                print_fix16(renderTime0, Time0);
                render_time();
            }
            break;

        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                Curve0 += val;
                if (Curve0 <= 0) Curve0 = 0;
                if (Curve0 >= 3) Curve0 = 3;
                ctl_param_change(eParamCurve0, Curve0);
                print_fix16(renderCurve0, Curve0);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Destination0 += val * 65536;
                    if (Destination0 < 0) Destination0 = 0;
                    ctl_param_change(eParamCurveDest0, Destination0);
                    print_fix16(renderDest0, Destination0);
                    render_dest();
                }
            break;

        case 3:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    //destination default values; 0, follow next source, 10v
                }
            break;

        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Time1 += val * 8; //5ms @ 48k
                if (Time1 < 0) Time1 = 0;
                ctl_param_change(eParamCurveTime1, Time1);
                print_fix16(renderTime1, Time1);
                render_time();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Curve1 += val;
                if (Curve1 <= 0) Curve1 = 0;
                if (Curve1 >= 3) Curve1 = 3;
                ctl_param_change(eParamCurve1, Curve1);
                print_fix16(renderCurve1, Curve1);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Destination1 += val * 65536;
                if (Destination1 < 0) Destination1 = 0;
                ctl_param_change(eParamCurveDest1, Destination1);
                print_fix16(renderDest1, Destination1);
                render_dest();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                //destination default values; 0, follow next source, 10v
            }
            break;
            
        default:
            break;
    }
}

void handle_encoder_2(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Time2 += val * 8; //5ms @ 48k
                if (Time2 < 0) Time2 = 0;
                ctl_param_change(eParamCurveTime2, Time2);
                print_fix16(renderTime2, Time2);
                render_time();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Curve2 += val;
                if (Curve2 <= 0) Curve2 = 0;
                if (Curve2 >= 3) Curve2 = 3;
                ctl_param_change(eParamCurve2, Curve2);
                print_fix16(renderCurve2, Curve2);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Destination2 += val * 65536;
                if (Destination2 < 0) Destination2 = 0;
                ctl_param_change(eParamCurveDest2, Destination2);
                print_fix16(renderDest2, Destination2);
                render_dest();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                //destination default values; 0, follow next source, 10v
            }
            break;
            
        default:
            break;
    }
}


void handle_encoder_3(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Time3 += val * 8; //5ms @ 48k
                if (Time3 < 0) Time3 = 0;
                ctl_param_change(eParamCurveTime3, Time3);
                print_fix16(renderTime3, Time3);
                render_time();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Curve3 += val;
                if (Curve3 <= 0) Curve3 = 0;
                if (Curve3 >= 3) Curve3 = 3;
                ctl_param_change(eParamCurve1, Curve3);
                print_fix16(renderCurve3, Curve3);
                render_curve();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Destination3 += val * 65536;
                if (Destination3 < 0) Destination3 = 0;
                ctl_param_change(eParamCurveDest3, Destination3);
                print_fix16(renderDest3, Destination3);
                render_dest();
            }
            break;
            
        case 3:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                //destination default values; 0, follow next source, 10v
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
