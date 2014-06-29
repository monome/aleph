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
#include "flash_prgm.h"
#include "handler.h"
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
    render_boot("run...");

    print_dbg("\r\n flash_prgm_init...");    
    flash_prgm_init();
    
    files_load_dsp_name();

    render_boot("loading prgm...");
    bfin_wait_ready();
    
    render_boot("enabling blackfin...");
    bfin_enable();
        
} else {
    print_dbg("\r\n app_launch NOT firstrun!...");      
    }

    render_boot("launching UI...");
        
    //encoder sensitivity
    set_enc_thresh(3, 16);
    delay_ms(20);
    
    //timers
    init_app_timers();

    //screen
    print_dbg("\r\n render_startup...");
    render_startup();
    
    //set app event handlers
    assign_prgm_event_handlers();
    print_dbg("\r\n return 1...");
    
    return 1;
}
