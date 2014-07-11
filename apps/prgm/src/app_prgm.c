//prgm
//aleph-avr32

// asf
#include "delay.h"
#include "gpio.h"
#include "print_funcs.h"

//avr32-lib
#include "app.h"
#include "bfin.h"
#include "flash.h"
#include "screen.h"

//prgm
#include "app_timers.h"
#include "files.h"
#include "pages.h"
#include "handler.h"
#include "ctl.h"
#include "render.h"
#include "encoders.h"


//hardware initialization, memory allocation
void app_init(void) {
    render_init();
}

u8 app_launch(u8 firstrun) {
if(firstrun) {
    print_dbg("\r\n app_launch firstrun...");    

    files_load_dsp();

    bfin_wait_ready();
            
} else {
    print_dbg("\r\n app_launch NOT firstrun...");
    files_load_dsp();

    bfin_wait_ready();
    }

    ctl_report_parameters();

    pages_init();
    
    bfin_enable();

    init_app_timers();
    
    adc_init();

    render_startup();
    
    assign_prgm_event_handlers();
        
    print_dbg("\r\n return 1...");
    return 1;
}
