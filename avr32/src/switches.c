/* switches.c
 * aleph
 */

// asf
#include "compiler.h"
#include "gpio.h"
#include "print_funcs.h"
// aleph
#include "aleph_board.h"
//#include "conf_aleph.h"
#include "events.h"
#include "event_types.h"
#include "global.h"
#include "switches.h"
#include "types.h"

static const U8 kSwitchPins[NUM_SW] = {
  SW0_PIN,
  SW1_PIN,
  SW2_PIN,
  SW3_PIN,
  SW_MODE_PIN,
  SW_POWER_PIN
};

static const eEventType kSwitchEvents[NUM_SW][2] = {
  { kEventSwitchDown0, kEventSwitchUp0 },
  { kEventSwitchDown1, kEventSwitchUp1 },
  { kEventSwitchDown2, kEventSwitchUp2 },
  { kEventSwitchDown3, kEventSwitchUp3 },
  { kEventSwitchDown4, kEventSwitchUp4 },
  { kEventSwitchDown5, kEventSwitchUp5 },
};

/*
// generate events from switch interrupts
void process_sw( const U8 swIdx )  {
  static event_t e;   

  //  print_dbg("\r\n processing switch : ");
  //  print_dbg_hex(swIdx);

  /// FIXME: bouncy bounce
  //  if( (tcTicks - swTicks[swIdx]) > 30) { 
   if(1) {
    e.eventType = kSwitchEvents[swIdx][gpio_get_pin_value(kSwitchPins[swIdx])];
    post_event(&e);
  }
  swTicks[swIdx] = tcTicks;
}
*/


//-----------------
// TEST: polling IO for switches until we get hardware deboucing

// debounce counters
static u32 swCount[NUM_SW];
// debounced results
static u8 swDebounce[NUM_SW] = { 0, 0, 0, 0, 0, 0 };

// initialize debouncing
void init_switches(void) {
  u8 i;
  for(i=0; i<NUM_SW; i++) {
    swCount[i] = SW_RELEASE_TICKS;
  }
}

// poll func
void poll_sw(const U8 idx) {
  static event_t e;
  u8 state = gpio_get_pin_value(kSwitchPins[idx]);
  u8 changed = 0;
  if(state == swDebounce[idx]) { 
    //    print_dbg("\r\n pollsw");
    // start counting no-change states
    if(swDebounce[idx]) {
      swCount[idx] = SW_RELEASE_TICKS;
    } else {
      swCount[idx] = SW_PRESS_TICKS;
    }
  } else {
    //    print_dbg("\r\n sw changed, count: ");
    //    print_dbg_hex(swCount[idx]);
    if(swCount[idx]-- == 0) {
      // timer expired, accept change
      swDebounce[idx] = state;
      changed = 1;
      // reset timer
      if(swDebounce[idx]) {
	swCount[idx] = SW_RELEASE_TICKS;
      } else {
	swCount[idx] = SW_PRESS_TICKS;
      }
    }
  }
  if(changed) {
    // spawn event
    e.eventType = kSwitchEvents[idx][state];
    post_event(&e);

    //print_dbg("\r\n posting SWITCH event, type: ");
    /* print_dbg_hex(e.eventType); */

  }
}
