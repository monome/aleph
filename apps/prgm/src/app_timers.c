//prgm
//aleph-avr32

/* app_timers.c
 a good strategy is to create application event types in app_events.h
 and have the timer callbacks post these events to the queue.
 the main loop will service the queue and call the application event handler.
*/

//asf
#include "print_funcs.h"

// avr32_lib
#include "encoders.h"
#include "events.h"
//#include "monome.h"
#include "timers.h"
//#include "midi.h"
#include "app_event_types.h"
#include "app_timers.h"
#include "control.h"
#include "render.h"

// tmp
static event_t e;

//------ timers
// refresh the screen periodically
static softTimer_t screenTimer  = { .next = NULL };

// poll encoders
static softTimer_t encTimer = { .next = NULL };

static softTimer_t metroTimer  = { .next = NULL };

// screen refresh callback
static void screen_timer_callback(void* obj) {  
    render_update();
}

// encoder accumulator polling callback
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

// metronome timer callback
static void metro_timer_callback(void* obj) {
    e.type = kEventAppCustom;
    event_post(&e);
}


//external functions
void init_app_timers(void) {
    timer_add(&screenTimer, 50,  &screen_timer_callback,  NULL);
    timer_add(&encTimer, 50, &enc_timer_callback, NULL );
    timer_add(&metroTimer, 1000,  &metro_timer_callback,  NULL);
}

// set the metro timer period
void timers_set_metro_ms(u32 ms) {
    // doing this will effectively pause the timer while knob moves...
    //  metroTimer.timeout = ms;
    metroTimer.ticks = ms;
}
