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

//static u8 inNotify = 0;
//static u8 notifyMsg = 0;
//static irqflags_t flags;

/* void app_notify(const char* msg) { */
/*   screen_clear(); */
/*   //  screen_line(0, 0, (char*)msg, 0xf); */
/*   screen_refresh(); */
/* } */

void app_pause(void) {
  /// test:
  //u32 i;
  //  event_t* sysevents;
  //  if (inNotify == 1) { return; }

  //  print_pending_events();

  //  delay_ns(100);
  delay_ms(10);

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);

  //  inNotify = 1;

  /// disable all interrupts and save flags 
  //  flags = cpu_irq_save();
    // enable pdca interrupt
  //  cpu_irq_enable_level(SYS_IRQ_PRIORITY);
}

void app_resume(void) {
  //  if(inNotify == 0) { return; }

  // disable pdca interrupt
  //  cpu_irq_disable_level(SYS_IRQ_PRIORITY);
  
  //  print_dbg("\r\n enabling tc/ui irq...");
  
  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);
  
  //  cpu_irq_restore(flags);
  // clear interrupt flag by reading timer SR
  // tc_read_sr(APP_TC, APP_TC_CHANNEL);
  
  //  print_dbg(" enbabled.");

  //  if(notifyMsg) {
  
  //  }
  
  //  notifyMsg = 0;
  //  inNotify = 0;
}
