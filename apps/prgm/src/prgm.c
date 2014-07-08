// prgm.c

// asf
#include "print_funcs.h"

// common
#include "fix.h"

// avr32
#include "region.h"
#include "app.h"
#include "control.h"

//prgm
#include "handler.h"
#include "pages.h"
#include "render.h"
#include "ctl.h"
#include "util.h"
#include "prgm.h"

//static functions
//static void handle_encoder_0(s32 val);

//static void handle_encoder_1(s32 val);

//static void handle_encoder_2(s32 val);

//static void handle_encoder_3(s32 val);

static void handle_key_4(s32 val);

/*
static etype touched = kNumEventTypes;
static u8 touchedThis = 0;

static inline u8 check_touch(etype et) {
    if(touched != et) {
        touchedThis = 1;
        touched = et;
    }
    return touchedThis;
}

//handlers

void handle_encoder_0(s32 val) {
    check_touch(kEventEncoder3);
    if (touchedThis) {
        Freq0 += val * 0x00010000;
        ctl_param_change(eParamFreq0, Freq0);
        print_fix16(renderFreq0, Freq0);
        render_freq();
    }
}

void handle_encoder_1(s32 val) {
    check_touch(kEventEncoder2);
    if (touchedThis) {
        Freq1 += val * 0x00010000;
        ctl_param_change(eParamFreq1, Freq1);
        print_fix16(renderFreq1, Freq1);
        render_freq();
    }
}

void handle_encoder_2(s32 val) {
    check_touch(kEventEncoder2);
    if (touchedThis) {
        Freq2 += val * 0x00010000;
        ctl_param_change(eParamFreq2, Freq2);
        print_fix16(renderFreq2, Freq2);
        render_freq();
    }
}

void handle_encoder_3(s32 val) {
    check_touch(kEventEncoder2);
    if (touchedThis) {
        Freq3 += val * 0x00010000;
        ctl_param_change(eParamFreq3, Freq3);
        print_fix16(renderFreq3, Freq3);
        render_freq();
    }
}

*/ 
void handle_key_4(s32 val) {
    if(val == 0) { return; }
    
    else {
        set_page(ePageTracker);
        render_tracker();
    }
}

void select_prgm(void) {
//assign prgm handlers
//    app_event_handlers[ kEventEncoder0 ]	= &handle_encoder_3 ;
//    app_event_handlers[ kEventEncoder1 ]	= &handle_encoder_2 ;
//    app_event_handlers[ kEventEncoder2 ]	= &handle_encoder_1 ;
//    app_event_handlers[ kEventEncoder3 ]	= &handle_encoder_0 ;
//    app_event_handlers[ kEventSwitch0 ]	= &handle_key_0 ;
//    app_event_handlers[ kEventSwitch1 ]	= &handle_key_1 ;
//    app_event_handlers[ kEventSwitch2 ]	= &handle_key_2 ;
//    app_event_handlers[ kEventSwitch3 ]	= &handle_key_3 ;
    app_event_handlers[ kEventSwitch4 ]	= &handle_key_4 ;
    
}
