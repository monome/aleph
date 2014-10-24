//handler.c
//aleph-prgm-avr32

//RENAME TO global.c

//asf
#include "print_funcs.h"

// avr32
#include "bfin.h"
#include "control.h"
#include "ctl.h"

#include "app.h"
#include "app_timers.h"
#include "types.h"

//prgm
#include "handler.h"
#include "render.h"

//static function declarations
static void handle_Adc0(s32 data);

//external functions
//init adc timers, called by app_launch()
void adc_init(void) {
    timers_set_adc(50);
}

//static functions
static void handle_Adc0(s32 data) {
//    step_advance();
}

void assign_prgm_event_handlers(void) {
    app_event_handlers[ kEventAdc0 ] = &handle_Adc0 ; //trig
//    app_event_handlers[ kEventAdc1 ] = &handle_Adc1 ; //sequencer +1
//    app_event_handlers[ kEventAdc2 ] = &handle_Adc2 ; //transpose +1
//    app_event_handlers[ kEventAdc3 ] = &handle_Adc3 ;
//    app_event_handlers[ kEventSwitch5 ]	= &handle_switch_5 ; //power switch
//    app_event_handlers[ kEventSwitch6 ] = &handle_switch_6 ; //sequence +1
}
