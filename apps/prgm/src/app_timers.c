//app_timers.c
//aleph-prgm-avr32

/*
this is where callbacks are declared for all application-specific software timers.
 these callbacks are performed from the TC interrupt service routine.
 therefore, they should be kept small.
 
 avr32_lib defines a custom application event type, 
 which should be used when timer-based processing needs to be deferred to the main loop.
*/

//asf
#include "print_funcs.h"

//aleph-avr32
#include "control.h"
#include "encoders.h"
#include "events.h"
#include "midi.h"
#include "timers.h"

//prgm and customized library
#include "adc.h"
#include "app_timers.h"
#include "render.h"

//---------------------------
//---- static variables

//EVENT
static event_t e;

//TIMERS
// poll adc
static softTimer_t adcPollTimer = { .next = NULL, .prev = NULL };

// refresh screen
static softTimer_t screenTimer = { .next = NULL, .prev = NULL };

// poll encoders
static softTimer_t encTimer = { .next = NULL, .prev = NULL };

//poll custom
static softTimer_t customPollTimer = { .next = NULL, .prev = NULL };

// poll monome device
static softTimer_t monomePollTimer = { .next = NULL, .prev = NULL };
// refresh monome device
static softTimer_t monomeRefreshTimer  = { .next = NULL, .prev = NULL };
// poll midi device
static softTimer_t midiPollTimer = { .next = NULL, .prev = NULL };

//adc polling callback
static void adc_poll_timer_callback(void* obj) {
//    adc_poll();
    adc_convert(&adc);
    
    u16 i = adc[0];

    if (i < 1)
//    if (!i)
        state = OFF;

    else if(state == OFF)
        {
            state = ON;
            
            e.type = kEventAdc0;
            e.data = ON;
            event_post(&e);
            
            e.type = kEventAdc0;
            e.data = OFF;
            event_post(&e);
        }
    
    else if(state == ON)
        ;
}

//encoder polling callback
static void enc_timer_callback(void* obj) {
    static s16 val, valAbs;
    u8 i;
    
    for(i=0; i<NUM_ENC; i++) {
        val = enc[i].val;
        valAbs = (val & 0x8000 ? (val ^ 0xffff) + 1 : val);
        if(valAbs > enc[i].thresh) {
            e.type = enc[i].event;
            e.data = val;
            enc[i].val = 0;
            event_post(&e);
        }
    }
}

// screen refresh callback
static void screen_timer_callback(void* obj) {
    render_update();
}

//external functions
void init_app_timers(void) {
    timer_add(&screenTimer, 50, &screen_timer_callback, NULL);
    timer_add(&encTimer, 50, &enc_timer_callback, NULL);
}

//start adc polling
void timers_set_adc(u32 period) {
    timer_add(&adcPollTimer, period, &adc_poll_timer_callback, NULL);
}

//set adc polling period
void timers_set_adc_period(u32 period) {
    adcPollTimer.ticks = period;
}
