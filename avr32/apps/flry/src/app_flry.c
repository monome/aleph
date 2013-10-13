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
#include "bfin.h"
#include "screen.h"
#include "events.h"
#include "event_types.h"
#include "flash.h"
#include "monome.h"

// flry
#include "app_timers.h"
#include "files.h"
#include "handler.h"
#include "ctl.h"
#include "inputs.h"
#include "render.h"

// this is called during hardware initialization.
// use for memory allocation..
void app_init(void) {
  // set handler
  appEventHandler = &flry_handler;
  render_init();
}

// this is called from the event queue to start the app 
// return >0 if there is an error doing firstrun init
u8 app_launch(u8 firstrun) {
  u32 waitForCard;

  print_dbg("\r\n app launch");
  print_dbg("\r\n firstrun: ");
  print_dbg_ulong(firstrun);

  if(firstrun) {
    // it is the first run.
    // need to copy audio module binary from sdcard to internal flash.
    render_boot("first run. waiting for SDcard...");
    render_update();
  
    print_dbg("\r\n SD check... ");
    while (!sd_mmc_spi_mem_check()) {
      waitForCard++;
    }
    print_dbg("\r\nfound SD card. ");

    render_boot("found sdcard.. reading DSP...");
    render_update();

    // search for our dsp and load it
    // return success (0 == fail)
    if( files_search_dsp() ) {
      ;;
    } else {
      screen_clear();
      return 0;
    }

  } else {

    // firstrun pattern was set, so there should be a blackfin executable in flash.
    // read from flash to RAM
    render_boot("loading flash to RAM...");
    render_update();
    flash_read_ldr();
    
    render_boot( "booting DSP from flash...");
    render_update();
    // reboot DSP from RAM
    bfin_load_buf();
  }

  render_boot("waiting for bfin init...      ");
  render_update();

  // this is retarded, we need the GPIO for bfin to signal when init done
  delay_ms(1000);

  // report parameters
  render_boot("reporting bfin params...       ");
  render_update();
  if( ctl_report_params() ) {
    ;;
  } else {
    render_boot("param report failed!           ");
    render_update();
    return 0;
  }

  render_boot("setting initial parameters...  ");
  render_update();

  ctl_init_params();

  render_boot("initializing input scalers...");
  render_update();
  
  inputs_init();

  delay_ms(20);

  // enable audio
  render_boot("run                       ");
  render_update();

  bfin_enable();

  // enable timers
  init_app_timers();

  render_startup();
  render_update();
  
  return 1;
}
