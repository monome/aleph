/*
  app_blank.c

  aleph/apps/blank

  required app-specific implementation of avr32_lib/src/app.h

  this blank application template does nothing but set up stubs.
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
#include "handler.h"
#include "ser.h"
#include "serial.h"
#include "app_timers.h"
#include "render.h"
#include "ctl.h"

#include "files.h"
#include "pico.h"

// this is called during hardware initialization.
// allocate memory here.
void app_init(void) {
  print_dbg("\r\n files_init...");
  files_init();

  print_dbg("\r\n render_init...");
  render_init();
}

// this is called from the event queue to start the app
// return >0 if there is an error doing firstrun init

u8 app_launch(eLaunchState firstrun) {
  //...
  print_dbg("\r\n mix; app_launch, firstrun: ");
  print_dbg_ulong(firstrun);

  print_dbg("\r\n initialising picolisp...");
  pil_init();
  serialInputLineBufferPos = 0;

  print_dbg("\r\n initialising serial...");
  serial_rx_flow_control = false;

  //// startup using default DSP name
  files_load_dsp_name("waves.ldr");

  bfin_wait_ready();

  //    print_dbg("\r\n enable DSP audio...");
  bfin_enable();


  // extra few ms...
  delay_ms(100);
  //==========================

  /* bfin_enable(); */

  // enable timers
  init_app_timers();

  render_startup();
  render_update();

  ctl_init();

  if (firstrun) {
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
