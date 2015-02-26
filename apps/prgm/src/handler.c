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

//prgm
#include "handler.h"
#include "render.h"
#include "tracker.h"

//static function declarations
static void handle_Adc0(s32 trig);
//static void handle_switch_6(s32 trig);
//static void handle_switch_7(s32 trig);

//external functions
//set adc timers, called by app_launch()
void adc_init(void) {
    timers_set_adc(1);
}

//static functions
void handle_Adc0(s32 trig) {
    if(trig)
    {
//        bfin_wait_ready();
        bfin_set_trig();
    }
    
    //  recording state 'arm': flash led on step trig
/*
    if(bfinheadstate == 1)
    {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
*/
}

static void handle_switch_5(s32 data) {
    //  check if mode switch is held
    if(gpio_get_pin_value(SW_MODE_PIN)) {
        ;;
    } else {
        //  write current to flash can be done here...
        ;;
    }

    delay_ms(10);
    render_boot("ghost in the machine...");

    // this pin is physically connected to the power system.
    // bringing it low causes immediate shutdown
    gpio_clr_gpio_pin(POWER_CTL_PIN);
}

/*
void handle_switch_6(s32 trig) {
    
    //  if trig, foot not touched
    if (trig && foot1_touched == OFF)
    {
        //  foot touched: update state
        foot1_touched = ON;
        
        //  recording state 'off': start recording to buffer
        if (!bfinheadstate)
        {
                ctl_param_change(0, eParamToggleRec, ON);
                bfinheadstate = ON;
                gpio_clr_gpio_pin(LED_MODE_PIN);
        }
        
        //  recording state 'on': wait...
        else ;
    }
    
    //  if trig, foot touched
    else if (trig && foot1_touched == ON)
    {
        //  foot touched: update state
        foot1_touched = OFF;
        
        //  recording state 'off': recording stopped by bfin
        if (!bfinheadstate) ;
            
        //  recording state 'on': stop recording to buffer
        else
        {
            ctl_param_change(0, eParamToggleRec, OFF);
            bfinheadstate = OFF;
            gpio_set_gpio_pin(LED_MODE_PIN);
        }
    }
}

void handle_switch_7(s32 trig) {    

    //  if trig, foot not touched
    if (trig && foot2_touched == OFF)
    {
        //  foot touched: update state
        foot2_touched = ON;
        
        //  recording state 'off': change recording state to 'arm' or 'on'
        if (!bfinheadstate)
        {
            //  bfinheadpos at pattern start: change recording and led state to 'on'
            if (!bfinheadpos) {
                //  start recording to buffer
                ctl_param_change(0, eParamToggleRec, ON);
                bfinheadstate = ON;
                gpio_clr_gpio_pin(LED_MODE_PIN);
            }
            //  bfinheadpos not at pattern start: set recording state to 'arm'
            else bfinheadstate = ARM;
        }
        
        //  recording state 'arm': change recording state to 'on' or wait...
        else if (bfinheadstate == ARM)
        {
            //  bfinplaypos at pattern start: set recording and led state to 'on'
            if (!bfinheadpos) {
                ctl_param_change(0, eParamToggleRec, ON);
                bfinheadstate = ON;
                gpio_clr_gpio_pin(LED_MODE_PIN);
            }
            //  bfinplaypos not at pattern start: wait for next step
            else ;
        }
        
        //  recording state 'on': wait for recording state to change
        else if (bfinheadstate == ON) ;
    }
    
    //  if trig, foot touched
    else if (trig && foot2_touched == ON)
    {
        //  foot touched: update state
        foot2_touched = OFF;
        
        //  recording state 'off': recording stopped by bfin
        if (!bfinheadstate)
        {
            gpio_set_gpio_pin(LED_MODE_PIN);
        }
        //  recording state 'arm': cancel recording prematurely
        if (bfinheadstate == ARM)
        {
            ctl_param_change(0, eParamToggleRec, OFF);
            bfinheadstate = OFF;
            gpio_set_gpio_pin(LED_MODE_PIN);
        }
        
        //  recording state 'on': stop recording to buffer
        else if (bfinheadstate == ON)
        {
            ctl_param_change(0, eParamToggleRec, OFF);
            bfinheadstate = OFF;
            gpio_set_gpio_pin(LED_MODE_PIN);
        }
    }
}
*/
 
void assign_prgm_event_handlers(void) {
    app_event_handlers[ kEventAdc0 ] = &handle_Adc0 ; //trig
//    app_event_handlers[ kEventAdc1 ] = &handle_Adc1 ;
//    app_event_handlers[ kEventAdc2 ] = &handle_Adc2 ;
//    app_event_handlers[ kEventAdc3 ] = &handle_Adc3 ;
    app_event_handlers[ kEventSwitch5 ]	= &handle_switch_5 ; //power switch
//    app_event_handlers[ kEventSwitch6 ] = &handle_switch_6 ; //foot1
//    app_event_handlers[ kEventSwitch7 ] = &handle_switch_7 ; //foot2
//    app_event_handlers[ kEventAppCustom ] = &handle_custom ;
}
