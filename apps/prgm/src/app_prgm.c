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
//#include "flash_prgm.h"
#include "handler.h"
#include "ctl.h"
#include "render.h"
#include "encoders.h"


//hardware initialization, memory allocation
void app_init(void) {
    print_dbg("\r\n app_init...");
    print_dbg("\r\n render_init...");
    render_init();
}

u8 app_launch(u8 firstrun) {
if(firstrun) {
    print_dbg("\r\n app_launch firstrun...");    

    files_load_dsp();

    bfin_wait_ready();

    bfin_enable();
            
} else {
    print_dbg("\r\n app_launch NOT firstrun!...");      
    }

    print_dbg("\r\n pages_init...");
    pages_init();

    print_dbg("\r\n init_app_timers...");
    init_app_timers();

    print_dbg("\r\n prgm_event_handlers...");
    assign_prgm_event_handlers();
    
    ctl_report_parameters();
    
    render_startup();

    print_dbg("\r\n return 1...");
    return 1;
}
