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
#include "i2c.h"

//prgm
#include "app_timers.h"
#include "ctl.h"
#include "handler.h"
#include "render.h"

#include "files.h"
#include "pages.h"
#include "tracker.h"
#include "flash.h"
#include "screen.h"

//hardware init, all memory allocations go here
void app_init(void) {
    print_dbg("\r\n app_init...");
    
    samples_init();
    print_dbg("\r\n finished samples_init()...");
    
    render_init();
    
    //  initialize prgm data structs, set default values
    tracker_init();
    
    // initialize flash-management buffers
    print_dbg("\r\n flash_prgm_init...");
    flash_prgm_init();
}

//dsp init
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
    pages_init();
    
    bfin_disable();
    
    init_sample_timer();
    
    print_dbg("\r\n n_samples ");
    print_dbg_ulong(n_samples);
    
    render_boot("loading samples...");
    
    for (smpl = N_BUFFERS + 1; smpl < n_samples; smpl++)
    {
        render_boot(sample_path[smpl-9]);
        files_load_sample(smpl);
        delay_ms(10);
        free_mem(bfinSampleData);
    }

//    init_scrub_timer();
    
    bfin_enable();
    
    init_app_timers();
    
    adc_init();

    render_startup();
    
    assign_prgm_event_handlers();
    
    print_dbg("\r\n return 1...");
    
    return 1;
}
