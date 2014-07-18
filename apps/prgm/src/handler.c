//handler.c
//aleph-prgm-avr32

//RENAME TO global.c

//asf
#include "print_funcs.h"

// avr32
#include "bfin.h"
#include "control.h"

#include "app.h"
#include "app_timers.h"
#include "types.h"

//prgm
#include "handler.h"
#include "render.h"
#include "ctl.h"


//static functions
//static void handle_AppCustom(s32 data) {
//    seq_advance();
//}

//static functions
static void handle_Adc0(s32 data);
//static void handle_switch_6(s32 data);
//static void handle_switch_7(s32 data);

void handle_Adc0(s32 data) {
    if(data == 0)
        state = OFF;
    
    else if(state == OFF) {
        state = ON;
        
        ctl_param_change(eParamSyncTrig, 1);
        ctl_param_change(eParamSyncTrig, 0);
    }
    
    else if(state == ON)
        ;
}

//MAKE THESE GLOBAL!
//void handle_switch_6(s32 data) {
    //step +1
//}

//void handle_switch_7(s32 data) {
    //return to 1
//}

//external functions
void adc_init() { //called by app_launch()
    timers_set_adc(10);
}

void assign_prgm_event_handlers(void) {

    app_event_handlers[ kEventAdc0 ] = &handle_Adc0 ; //sync trig
//    app_event_handlers[ kEventAdc1 ] = &handle_Adc1 ;
//    app_event_handlers[ kEventAdc2 ] = &handle_Adc2 ;
//    app_event_handlers[ kEventAdc3 ] = &handle_Adc3 ;
//    app_event_handlers[ kEventSwitch5 ]	= &handle_switch_5 ; //power switch
//    app_event_handlers[ kEventSwitch6 ]	= &handle_switch_6 ; //sequencer +1
//    app_event_handlers[ kEventSwitch7 ]	= &handle_switch_7 ; //sequencer restart at 1
//    app_event_handlers[ kEventAppCustom ]	= &net_poll_handler ;
}
