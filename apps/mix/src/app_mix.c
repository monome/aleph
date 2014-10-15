/*
  app_bees.c
  aleph-avr32
  bees

  required app-specific implementation of avr32/src/app.h
[ */

// asf
#include "delay.h"
#include "gpio.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"

// aleph-avr32
#include "app.h"
#include "app_timers.h"
#include "bfin.h"
#include "screen.h"
#include "encoders.h"
#include "events.h"
#include "event_types.h"
#include "flash.h"
#include "monome.h"

//#include "files.h"
#include "handler.h"
#include "ctl.h"

#include "render.h"

// this is called during hardware initialization.
// use for memory allocation..
void app_init(void) {
  render_init();
}

// this is called from the event queue to start the app 
// return >0 if there is an error doing firstrun init
u8 app_launch(u8 firstrun) {

  render_boot("waiting for bfin init...      ");
  render_update();


  //=============================

  /* load the DSP module here.
     the avr32 library maintains a designated static RAM buffer for this.
     it's completely wasteful here, since we have the .ldr in flash.
     we'll use this interface for now,
     but it would be more efficient to use our own copy of bfin.c,
     and modify the relevant routines
  */

  //==========================

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
    //...
  }

  // set app event handlers
  assign_event_handlers();

  return 1;
}
