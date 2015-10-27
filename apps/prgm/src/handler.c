//handler.c
//aleph-prgm-avr32

//RENAME TO global.c

//asf
#include "print_funcs.h"

// avr32
#include "delay.h"
#include "bfin.h"
#include "control.h"
#include "ctl.h"

#include "app.h"
#include "app_timers.h"
#include "types.h"
#include "i2c.h"

//prgm
#include "handler.h"
#include "render.h"
#include "tracker.h"
#include "generator.h"


//static function declarations
static void handle_Adc0(s32 trig);
//static void handle_Adc1(s32 gate);
static void handle_switch_5(s32 data);


//static functions
void handle_Adc0(s32 trig) {
    if(trig) bfin_set_clock_in();
}

/*
void handle_Adc1(s32 gate) {
    if(gate) bfin_set_gate_out();
}
*/
 
void handle_switch_5(s32 data) {
    //  check if mode switch is held
    if(gpio_get_pin_value(SW_MODE_PIN)) {
        ;;
    } else {
        //  write current to flash can be done here...
        ;;
    }

    delay_ms(10);
    render_boot("ghost in the machine...");
    delay_ms(1);

    // this pin is physically connected to the power system.
    // bringing it low causes immediate shutdown
    gpio_clr_gpio_pin(POWER_CTL_PIN);
}


void assign_prgm_event_handlers(void) {
    app_event_handlers[ kEventAdc0 ] = &handle_Adc0 ; //trig
//    app_event_handlers[ kEventAdc1 ] = &handle_Adc1 ; //gate
    app_event_handlers[ kEventSwitch5 ]	= &handle_switch_5 ; //power switch
}
