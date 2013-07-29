// asf
#include "print_funcs.h"
#include "tc.h"
// aleph
#include "app.h"
#include "aleph_board.h"
#include "screen.h"

//static u8 inNotify = 0;
//static u8 notifyMsg = 0;

void app_notify(const char* msg) {
  screen_clear();
  screen_line(0, 0, (char*)msg, 0xf);
  screen_refresh();
}

void app_pause(void) {
  
  //  if (inNotify == 1) { return; }

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);
  //  inNotify = 1;

}

void app_resume(void) {
  //  if(inNotify == 0) { return; }

  print_dbg("\r\n enabling tc/ui irq...");

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_enable_level(UI_IRQ_PRIORITY);

  // clear interrupt flag by reading timer SR
  tc_read_sr(APP_TC, APP_TC_CHANNEL);


  print_dbg(" enbabled.");

  //  if(notifyMsg) {
     
  //  }

  //  notifyMsg = 0;
  //  inNotify = 0;
}
