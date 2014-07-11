// prgm.c

// asf
#include "print_funcs.h"

#include "events.h"
#include "event_types.h"

// common
#include "fix.h"

//#include "fract32_emu.h"

// avr32
#include "region.h"
#include "app.h"
#include "bfin.h"
#include "control.h"

//prgm
#include "handler.h"
#include "pages.h"
#include "render.h"
#include "ctl.h"
#include "util.h"
#include "prgm.h"

//static functions
//static void handle_AppCustom(s32 data) {
//    seq_advance();
//}

static inline void handle_sw(u8 id, u8 b);

static void handle_switch_0(s32 data);

static void handle_switch_1(s32 data);

static void handle_switch_3(s32 data);

static void handle_switch_4(s32 val);

static void handle_encoder_0(s32 val);

static void handle_encoder_1(s32 val);

static void handle_encoder_2(s32 val);

static void handle_encoder_3(s32 val);


static etype touched = kNumEventTypes;
static u8 touchedThis = 0;

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

void handle_switch_3(s32 data) {
    //nothing
}

void handle_encoder_0(s32 val) {
//    static event_t e;
    
    switch (state_sw) {
        case 0:
            break;
        case 1:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Phase0 += val * 64;
                    ctl_param_change(eParamTripPoint0, Phase0 & 0xffff);
                    print_fix16(renderPhase0, Phase0);
                    render_phase();
                }
            break;
        
        case 2:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Blend0 += val * 64;
                    ctl_param_change(eParamWave0, Blend0 & 0xffff);
                    print_fix16(renderBlend0, Blend0);
                    render_blend();
                }
        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    switch (state_sw) {
        case 0:
            break;
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Phase1 += val * 64;
                ctl_param_change(eParamTripPoint1, Phase1 & 0xffff);
                print_fix16(renderPhase1, Phase1);
                render_phase();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Blend1 += val * 64;
                ctl_param_change(eParamWave1, Blend1 & 0xffff);
                print_fix16(renderBlend1, Blend1);
                render_blend();
            }
        default:
            break;
    }
}

void handle_encoder_2(s32 val) {
    switch (state_sw) {
        case 0:
            break;
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Phase2 += val * 64;
                ctl_param_change(eParamTripPoint2, Phase2 & 0xffff);
                print_fix16(renderPhase2, Phase2);
                render_phase();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Blend2 += val * 64;
                ctl_param_change(eParamWave2, Blend2 & 0xffff);
                print_fix16(renderBlend2, Blend2);
                render_blend();
            }
        default:
            break;
    }
}

void handle_encoder_3(s32 val) {
    switch (state_sw) {
        case 0:
            break;
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Phase3 += val * 64;
                ctl_param_change(eParamTripPoint3, Phase3 & 0xffff);
                print_fix16(renderPhase3, Phase3);
                render_phase();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Blend3 += val * 64;
                ctl_param_change(eParamWave3, Blend3 & 0xffff);
                print_fix16(renderBlend3, Blend3);
                render_blend();
            }
        default:
            break;
    }
}

/*
 void handle_key_3(s32 val) {
 s32 tmp;
 if(val == 0) { return; }
 
 else if (counter < N_STEPS - 1) {
 tmp = Freq0;
 step[counter]->osc_f = tmp;
 counter++;
 Freq0 = step[counter]->osc_f;
 ctl_param_change(eParamFreq0, Freq0);
 print_fix16(renderFreq0, Freq0);
 render_freq();
 }
*/
//#define FR32_MAX      0x7fffffff
//#define FR32_MIN      0x80000000

//static inline fract16 param_unit_to_fr16(ParamValue v);
//static inline fract32 param_unit_to_fr32(ParamValue v);

//static inline fract16 param_unit_to_fr16(ParamValue v) {
//    return (fract16)((v & 0xffff) >> 1);
//}

//static inline fract32 param_unit_to_fr32(ParamValue v) {
//    return fr16_to_fr32((fract16)((v & 0xffff) >> 1));
//}

void handle_switch_4(s32 val) {
    if(val == 0) { return; }
    
    else {
        set_page(ePageTracker);
        render_tracker();
    }
}

void select_prgm(void) {
//assign prgm handlers
    app_event_handlers[ kEventEncoder0 ]	= &handle_encoder_3 ;
    app_event_handlers[ kEventEncoder1 ]	= &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder2 ]	= &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder3 ]	= &handle_encoder_0 ;
    app_event_handlers[ kEventSwitch0 ]     = &handle_switch_0 ;
    app_event_handlers[ kEventSwitch1 ]     = &handle_switch_1 ;
//    app_event_handlers[ kEventSwitch2 ]	= &handle_key_2 ;
    app_event_handlers[ kEventSwitch3 ]     = &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ]     = &handle_switch_4 ;
    
}
