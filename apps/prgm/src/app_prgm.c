//app_prgm.c
//aleph-prgm-avr32

// asf
#include "delay.h"
#include "gpio.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"

//avr32-lib
#include "app.h"
#include "bfin.h"
#include "encoders.h"

//prgm
#include "app_timers.h"
#include "ctl.h"
#include "handler.h"            //adc_init
#include "render.h"             //prgm_init

#include "files.h"              //files_load_dsp
#include "pages.h"
#include "scale.h"              //scale_init
#include "tracker.h"
#include "flash.h"
#include "screen.h"

//hardware init, all memory allocations go here
void app_init(void) {
    print_dbg("\r\n app_init...");

    samples_init();
    print_dbg("\r\n finished samples_init()...");
    
    scale_init();
    
    render_init();
    
    tracker_init();
}

//dsp init
u8 app_launch(u8 firstrun) {
if(firstrun) {
    print_dbg("\r\n app_launch firstrun...");
    
    files_load_samples();
    print_dbg("\r\n finished files_load_samples()...");
    
    files_load_dsp();
    
    bfin_wait_ready();
    
    
} else {
    print_dbg("\r\n app_launch NOT firstrun...");

    files_load_samples();
    print_dbg("\r\n finished files_load_samples()...");
    
    files_load_dsp();

    bfin_wait_ready();    
}
    pages_init();
    
    bfin_enable();
    
    init_app_timers();
    
    adc_init();
    
    render_startup();
    
    assign_prgm_event_handlers();
    
//    print_dbg("\r\n sizeof prgmTrack");
//    print_dbg_ulong(sizeof(prgmTrack));
    
    print_dbg("\r\n return 1...");
    return 1;
}
