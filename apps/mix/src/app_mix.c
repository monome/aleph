/*
  app_mix.c

  aleph/app/mix

  required app-specific implementation of avr32_lib/src/app.h
*/

// asf
#include "delay.h"
#include "gpio.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"

// avr32_lib
#include "app.h"
#include "bfin.h"
#include "encoders.h"

//--- app-specific
#include "app_timers.h"
#include "ctl.h"
#include "handler.h"
#include "render.h"

// byte array holding the blackfin LDR data.
static const u8 ldrData[] = { 
#include "aleph-mix.ldr.inc"
};

static const u32 ldrSize = 
#include "aleph-mix.ldr_size.inc"
  ;

// this is called during hardware initialization.
// allocate memory here.
void app_init(void) {
  print_dbg("\r\n mix; app_init...");  
  render_init();
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

  bfin_load_buf(ldrData, ldrSize);
  bfin_wait_ready();

  // extra few ms...
  delay_ms(10);
  //==========================

  // enable audio
  bfin_enable();

  // enable timers
  init_app_timers();

  // render initial screen
  render_startup();
  render_update();

  // set hardcoded default values 
  ctl_init();

  if(firstrun) { 
    // first run since module was flashed.
  } else {
    // this wasn't the first run, so try and load last saved settings
    //... TODO...
  }

  // set app event handlers
  assign_event_handlers();

  // tell the main loop that we launched successfully.
  // if this was the first run, 
  // main() should now write the firstrun pattern to flash and reboot.
  return 1;
}
