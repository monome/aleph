#include "app.h"
#include "aleph_board.h"
#include "screen.h"

static u8 inNotify = 0;
static u8 notifyMsg = 0;

void app_notify(const char* startMsg, const char* stopMsg) {
  // show a message
}

void app_pause(void) {
  
  if (inNotify == 1) { return; }

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  cpu_irq_disable_level(UI_IRQ_PRIORITY);
   inNotify = 1;

 }

 void app_resume(void) {
   if(inNotify == 0) { return; }

   cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
   cpu_irq_enable_level(UI_IRQ_PRIORITY);

   if(notifyMsg) {
     
   }

  notifyMsg = 0;
  inNotify = 0;
}
