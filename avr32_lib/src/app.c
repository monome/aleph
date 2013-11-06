// asf
#include "print_funcs.h"
#include "tc.h"
// aleph
#include "app.h"
#include "aleph_board.h"
#include "conf_tc_irq.h"
#include "delay.h"
#include "events.h"
#include "screen.h"

// global array of pointers to handlers
void (*app_event_handlers[kNumSysEvents])(s32 data);


// pause/resume functions
void app_pause(void) {
  //  delay_ns(100);
  //  delay_ms(10);

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  /// disable all interrupts and save flags 
  //  flags = cpu_irq_save();

}

void app_resume(void) {
  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);  

  //  cpu_irq_restore(flags);
}
