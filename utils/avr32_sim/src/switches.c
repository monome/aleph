/* switches.c
 * aleph
 */

// asf
//#include "compiler.h"
//#include "gpio.h"
#include "print_funcs.h"
// aleph
//#include "aleph_board.h"
#include "events.h"
#include "event_types.h"
#include "global.h"
#include "switches.h"
#include "types.h"

/* static const U8 kSwitchPins[NUM_SW] = { */
/*   SW0_PIN, */
/*   SW1_PIN, */
/*   SW2_PIN, */
/*   SW3_PIN, */
/*   SW_MODE_PIN, */
/*   SW_POWER_PIN, */
/*   FS0_PIN, */
/*   FS1_PIN, */
/* }; */

#if 0
static const etype kSwitchEvents[NUM_SW] = {
  kEventSwitch0, 
  kEventSwitch1,
  kEventSwitch2,
  kEventSwitch3,
  kEventSwitch4,
  kEventSwitch5,
  kEventSwitch6,
  kEventSwitch7,
};
#endif

// generate events from switch interrupts
void process_sw( const U8 swIdx )  {
#if 1
#else
  static event_t e;   
  e.type = kSwitchEvents[swIdx];
  e.data = gpio_get_pin_value(kSwitchPins[swIdx]); 
  event_post(&e);
  //  print_dbg("\r\n posted switch event ");
  //  print_dbg_ulong(swIdx);
#endif
}
