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
#include "app.h" //remove
#include "control.h"

//prgm
#include "prgm.h"
#include "ctl.h"
#include "render.h"
#include "handler.h"
#include "pages.h"
#include "util.h" //remove
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
static etype touched = kNumEventTypes;
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
    //nothing
}

void handle_switch_3(s32 data) {
    //nothing
}

void handle_encoder_0(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Wave0 += val;
                    if (Wave0 < 0) Wave0 = 0;
                    if (Wave0 > numwaves - 1) Wave0 = numwaves - 1;

                    files_load_wavetable(Wave0);
                    print_dbg("\r\n finished files_load_wavetable... ");
                    
                    ctl_wavetable_change();
                    print_dbg("\r\n finished bfin transfer... ");
                    
                    print_fix16(renderWave0, Wave0);
                    render_wave();
                }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Phase0 += val * 128;
                    if (Phase0 < 0) Phase0 = 0;
                    ctl_param_change(eParamTripPoint0, Phase0 & 0xffff);
                    print_fix16(renderPhase0, Phase0);
                    render_phase();
                }
            break;
        
        case 2:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Blend0 += val * 128;
                    if (Blend0 < 0) Blend0 = 0;
                    ctl_param_change(eParamWave0, Blend0 & 0xffff);
                    print_fix16(renderBlend0, Blend0);
                    render_blend();
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
                Wave1 += val;
                if (Wave1 < 1) Wave1 = 0;
                if (Wave1 > 1) Wave1 = 2;
                ctl_param_change(eParamTab1, Wave1);
                print_fix16(renderWave1, Wave1);
                render_wave();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Phase1 += val * 128;
                if (Phase1 < 0) Phase1 = 0;
                ctl_param_change(eParamTripPoint1, Phase1 & 0xffff);
                print_fix16(renderPhase1, Phase1);
                render_phase();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Blend1 += val * 128;
                if (Blend1 < 0) Blend1 = 0;
                ctl_param_change(eParamWave1, Blend1 & 0xffff);
                print_fix16(renderBlend1, Blend1);
                render_blend();
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
                Wave2 += val;
                if (Wave2 < 1) Wave2 = 0;
                if (Wave2 > 1) Wave2 = 2;
                ctl_param_change(eParamTab2, Wave2);
                print_fix16(renderWave2, Wave2);
                render_wave();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Phase2 += val * 128;
                if (Phase2 < 0) Phase2 = 0;
                ctl_param_change(eParamTripPoint2, Phase2 & 0xffff);
                print_fix16(renderPhase2, Phase2);
                render_phase();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Blend2 += val * 128;
                if (Blend2 < 0) Blend2 = 0;
                ctl_param_change(eParamWave2, Blend2 & 0xffff);
                print_fix16(renderBlend2, Blend2);
                render_blend();
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
                Wave3 += val;
                if (Wave3 < 1) Wave3 = 0;
                if (Wave3 > 1) Wave3 = 2;
                ctl_param_change(eParamTab3, Wave3);
                print_fix16(renderWave3, Wave3);
                render_wave();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Phase3 += val * 128;
                if (Phase3 < 0) Phase3 = 0;
                ctl_param_change(eParamTripPoint3, Phase3 & 0xffff);
                print_fix16(renderPhase3, Phase3);
                render_phase();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Blend3 += val * 128;
                if (Blend3 < 0) Blend3 = 0;
                ctl_param_change(eParamWave3, Blend3 & 0xffff);
                print_fix16(renderBlend3, Blend3);
                render_blend();
            }
            break;
            
        default:
            break;
    }
}

void handle_switch_4(s32 data) {
    if(data == 0) { return; }
    
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
    app_event_handlers[ kEventSwitch2 ]     = &handle_switch_2 ;
    app_event_handlers[ kEventSwitch3 ]     = &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ]     = &handle_switch_4 ;
}
