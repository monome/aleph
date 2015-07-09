/* app_timers.c

   this is where callbacks are declared for all application-specific software timers.
   these callbacks are performed from the TC interrupt service routine.
   therefore, they should be kept small.

   avr32_lib defines a custom application event type, 
   which should be used when timer-based processing needs to be deferred to the main loop.
*/

//asf
#include "print_funcs.h"

// aleph-avr32
#include "adc.h"
#include "control.h"
#include "encoders.h"
#include "events.h"
#include "midi.h"
#include "monome.h"
#include "timers.h"

// bees
#include "app_timers.h"
#include "render.h"

//---------------------------
//---- static variables

// event 
static event_t e;

//------ timers
// refresh screen
static softTimer_t screenTimer = { .next = NULL, .prev = NULL };

// poll encoders
static softTimer_t encTimer = { .next = NULL, .prev = NULL };


//--------------------------
//----- static functions

//----- callbacks

// screen refresh callback
static void screen_timer_callback(void* obj) {  
  render_update();
}

// encoder accumulator polling callback
// the encoder class is defined in avr32_lib.
// each encoder object has an accumulator
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

//----------------------------
//---- external functions

void init_app_timers(void) {
  timer_add(&screenTimer, 50, &screen_timer_callback, NULL );
  timer_add(&encTimer, 50, &enc_timer_callback, NULL );
}
