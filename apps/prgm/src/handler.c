//prgm
//aleph-avr32

//asf
#include "print_funcs.h"

// avr32
#include "control.h"
#include "events.h"
#include "event_types.h"
#include "interrupts.h"
#include "screen.h"

//prgm
#include "handler.h"
#include "ctl.h"
#include "app_timers.h"
#include "render.h"

static etype touched = kNumEventTypes;
static u8 touchedThis = 0;

static inline void check_touch(etype et) {
    if(touched != et) {
        touchedThis = 1;
        touched = et;
    }
}

static void handle_Encoder0(s32 data) {
    check_touch(kEventEncoder0);
    if(touchedThis) {
    ctl_eParamFreq0(0, data); //declared in ctl.h
    }        
}




//external functions
void prgm_assign_event_handlers(void) {
    app_event_handlers[ kEventEncoder0 ] = &handle_Encoder0 ;
//    app_event_handlers[ kEventEncoder1 ] = &handler_Encoder1 ;
//    app_event_handlers[ kEventEncoder2 ] = &handler_Encoder2 ;
//    app_event_handlers[ kEventEncoder3 ] = &handler_Encoder3 ;
//    app_event_handlers[ kEventSwitch0 ]	= &handler_Switch0 ;
//    app_event_handlers[ kEventSwitch1 ]	= &handler_Switch1 ;
//    app_event_handlers[ kEventSwitch2 ]	= &handler_Switch2 ;
//    app_event_handlers[ kEventSwitch3 ]	= &handler_Switch3 ;
//    app_event_handlers[ kEventSwitch4 ]	= &handler_Switch4 ; //mode switch
//    app_event_handlers[ kEventSwitch5 ]	= &handler_Switch5 ; //power switch
//    app_event_handlers[ kEventSwitch6 ]	= &handler_Switch6 ; //foot
//    app_event_handlers[ kEventSwitch7 ]	= &handler_Switch7 ; //the other foot
}
