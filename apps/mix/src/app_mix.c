/*
  app_mix.c

  aleph/app/mix

  required app-specific implementation of avr32_lib/src/app.h
  [ */

// asf
#include "delay.h"
#include "gpio.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
//#include "events.h"
//#include "event_types.h"
//#include "encoders.h"
//#include "flash.h"
//#include "screen.h"

//#include "files.h"
#include "ctl.h"
#include "handler.h"
#include "render.h"

#if 1
static const u8 ldrData[] = { 
#include "aleph-mix.ldr.inc"
};

static const u32 ldrSize = 
#include "aleph-mix.ldr_size.inc"
  ;
#endif

// this is called during hardware initialization.
// use for memory allocation..
void app_init(void) {
  print_dbg("\r\n mix; app_init...");  
  //  render_init();
}

// this is called from the event queue to start the app 
// return >0 if there is an error doing firstrun init
u8 app_launch(u8 firstrun) {
  print_dbg("\r\n mix; app_launch, firstrun: ");
  print_dbg_ulong(firstrun);
  //=============================

  /* load the DSP module!
     
     in this app, the .ldr file is included directly,
     as a static const array of bytes.
  */
  print_dbg("\r\n mix; LDR size: 0x");
  print_dbg_hex(ldrSize);

  //  bfin_load_buf(ldrData, ldrSize);

  //==========================

# if 0
  bfin_wait_ready();

  // set encoder sensitivity
  set_enc_thresh(3, 16);
  delay_ms(20);

  // enable audio
  bfin_enable();

  // enable timers
  init_app_timers();

  // render initial screen
  render_startup();
  render_update();

  if(firstrun) { 
    // first run since module was flashed.
    // set hardcoded default values.
  } else {
    // this wasn't the first run, so try and load last saved settings
    //... TODO...
  }

  // set app event handlers
  assign_event_handlers();
  return 1;
#endif

  // tell the main loop that we launched successfully.
  // if this was the first run, 
  // main() should now write the firstrun pattern to flash and reboot.
  return 1;

}
