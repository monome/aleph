/* switches.c
 * aleph
 */

// asf
#include "compiler.h"
#include "gpio.h"
// aleph
#include "conf_aleph.h"
#include "events.h"
#include "event_types.h"
#include "global.h"
#include "switches.h"
#include "types.h"

static u64 swTicks[NUM_SW] = { 0, 0, 0, 0, 0 };

static const U8 kSwitchPins[NUM_SW] = {
  SW0_PIN,
  SW1_PIN,
  SW2_PIN,
  SW3_PIN,
  SW_EDIT_PIN
};

static const eEventType kSwitchEvents[NUM_SW][2] = {
  { kEventSwitchDown0, kEventSwitchUp0 },
  { kEventSwitchDown1, kEventSwitchUp1 },
  { kEventSwitchDown2, kEventSwitchUp2 },
  { kEventSwitchDown3, kEventSwitchUp3 },
  { kEventSwitchDown4, kEventSwitchUp4 },
};

// generate events from switch interrupts
void process_sw( const U8 swIdx )  {
  static event_t e;   

  //  if( (tcTicks - swTicks[swIdx]) > 30) { 
  if(1) {
    e.eventType = kSwitchEvents[swIdx][gpio_get_pin_value(kSwitchPins[swIdx])];
    post_event(&e);
  }
  swTicks[swIdx] = tcTicks;
}
