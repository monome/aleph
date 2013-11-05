/* app_timers.c
   
   setup up application software timers.
   TODO: should really split some of these off from avr32 core to app
 */


// ASF
#include "compiler.h"
#include "delay.h"
#include "gpio.h"
#include "math.h"
#include "print_funcs.h"
#include "uhc.h"
// aleph-avr32
#include "adc.h"
#include "control.h"
#include "encoders.h"
#include "events.h"
#include "event_types.h"
#include "ftdi.h"
#include "midi.h"
#include "monome.h"
#include "global.h"
#include "switches.h"
#include "timers.h"
#include "types.h"
#include "uhi_ftdi.h"

#include "sys_timers.h"



//----- static variables

//--- timers
// poll encoder accumulators periodically
static swTimer_t encTimer;

//--- static misc
static u8 i;
static event_t e;
			       
//----- callbacks

// encoder accumulator polling callback
//// FIXME optimize
static void enc_timer_callback(int tag) {
  static s16 val;

  for(i=0; i<NUM_ENC; i++) {
    val = enc[i].val;

    /// FIXME: this comparison is obviously retarded
    if ( (val > enc[i].thresh) || (val < (enc[i].thresh * -1)) ) {
    //  if( (val > ENC_THRESH) || (val < ENC_NEG_THRESH) ) {
      e.type = enc[i].event;
      e.data = val;
      enc[i].val = 0;
      event_post(&e);
      //      print_dbg("\r\n posting encoder event, num: ");'
      //      print_dbg_ulong(i);
      //      print_dbg(" , val: ");
      //      print_dbg_ulong(val);
    }
  }
}

// param change callback
/* static void param_change_timer_callback(int tag) { */
/*   ctl_perform_all_changes(); */
/* } */


//====== external
void init_sys_timers(void) {
  set_timer(&encTimer,           eEncTimerTag,           20,  &enc_timer_callback,    1);
  //  set_timer(&paramChangeTimer,   eParamChangeTimerTag,      1,  &param_change_timer_callback, 1);
}
