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
    
    files_load_dsp();
    
    bfin_wait_ready();
    
} else {
    print_dbg("\r\n app_launch NOT firstrun...");
    
    files_load_dsp();

    bfin_wait_ready();
    
}
    pages_init();

    bfin_enable();
    
    init_sample_timer();
    
    for (smpl = 8; smpl < n_samples; smpl++)
    {
        files_load_sample(smpl);
        
        //  wait for transfer to finish...
        delay_ms(8000);
        if (sample[smpl]->size > 4000) delay_ms(8000);
        if (sample[smpl]->size > 16000) delay_ms(8000);
        
        free_mem(bfinSampleData);

        bfinSampleSize = 0;
        idx8 = 0;
        idx32 = 0;
    }
    
    deinit_sample_timer();
    
    init_app_timers();
    
    adc_init();
    
    render_startup();
    
    assign_prgm_event_handlers();
        
//    print_dbg("\r\n sizeof prgmTrack");
//    print_dbg_ulong(sizeof(prgmTrack));
    
    print_dbg("\r\n return 1...");
    return 1;
}
