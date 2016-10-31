// asf
#include "tc.h"
// aleph
#include "debug.h"

#include "app.h"
#include "conf_board.h"
#include "conf_tc_irq.h"
#include "delay.h"
#include "events.h"
#include "screen.h"

// global array of pointers to handlers
void (*app_event_handlers[kNumEventTypes])(s32 data);

// pause/resume functions

static volatile s8 app_pause_resume_nesting = 0;

void app_pause(void) {
    if (!app_pause_resume_nesting > 0) {
        cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
        cpu_irq_disable_level(UI_IRQ_PRIORITY);
        // print_dbg("\r\n ... APP PAUSED ... ");
    }
    ++app_pause_resume_nesting;

    // print_dbg("\r\n ... app_pause() nesting: ");
    // print_dbg_hex(app_pause_resume_nesting);
}

void app_resume(void) {
    --app_pause_resume_nesting;
    if (app_pause_resume_nesting == 0) {
        cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
        cpu_irq_enable_level(UI_IRQ_PRIORITY);
        // print_dbg("\r\n ... APP RESUMED ... ");
    }

    // print_dbg("\r\n ... app_resume() nesting: ");
    // print_dbg_hex(app_pause_resume_nesting);
}
