// asf

/* #include "print_funcs.h" */
/* #include "tc.h" */
// aleph
#include "app.h"
// #include "aleph_board.h"
//#include "conf_tc_irq.h"
//#include "delay.h"
#include "events.h"
#include "screen.h"

// global array of pointers to handlers
void (*app_event_handlers[kNumEventTypes])(s32 data);


// pause/resume functions
/// note: these are not very smart; 
// in particular, nesting multiple calls to pause/resume will not work as expected.

void app_pause(void) {
#if 1
#else
  //  print_dbg("\r\n ... APP PAUSED ... ");
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
 
 cpu_irq_disable_level(UI_IRQ_PRIORITY);
#endif
}

void app_resume(void) {
#if 1
#else
  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);  
 
 //  print_dbg("\r\n ... APP RESUMED ... ");
#endif
}
