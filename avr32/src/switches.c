/* switches.c
 * aleph
 */

// asf
#include "compiler.h"
#include "gpio.h"
// aleph
#include "conf_aleph.h"
#include "events.h"
#include "eventTypes.h"
#include "switches.h"

static const U8 kSwitchPins[] = {
  SW0_PIN,
  SW1_PIN,
  SW2_PIN,
  SW3_PIN
};

static const eEventType kSwitchEvents[4][2] = {
  { kEventSwitchDown0, kEventSwitchUp0 },
  { kEventSwitchDown1, kEventSwitchUp1 },
  { kEventSwitchDown2, kEventSwitchUp2 },
  { kEventSwitchDown3, kEventSwitchUp3 },
};

// generate events from switch interrupts
void process_sw( const U8 swIdx )  {
  event_t e;   
  e.eventType = kSwitchEvents[swIdx][gpio_get_pin_value(kSwitchPins[swIdx])];
  post_event(&e);
}
