/*
  app_bees.c
  aleph-avr32
  bees

  required app-specific implementation of avr32/src/app.h
 */

// asf
#include "delay.h"
#include "gpio.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "flash.h"
#include "monome.h"
#include "screen.h"

// lppr
#include "files.h"
#include "handler.h"
#include "ctl.h"
#include "render.h"

// this is called during hardware initialization.
// use for memory allocation.
void app_init(void) {
  // set handler
  appEventHandler = &lppr_handler;
  render_init();
}

// this is called from the event queue to start the app 
// return >0 if there is an error doing firstrun init
u8 app_launch(u8 firstrun) {
  u32 waitForCard;

  print_dbg("\r\n app launch");
  print_dbg("\r\n firstrun: ");
  print_dbg_ulong(firstrun);

  app_notify("launching LPPR");

  if(firstrun) {
    // it is the first run.
    // need to copy audio module binary from sdcard to internal flash.
    screen_line(0, 0, "firstrun.        ", 0x3f);
    screen_blank_line(0, 1);
    screen_line(0, 1, "waiting for SD card...", 0x3f);
    screen_refresh();
  
    print_dbg("\r\n SD check... ");
    while (!sd_mmc_spi_mem_check()) {
      waitForCard++;
    }
    print_dbg("\r\nfound SD card. ");

    screen_blank_line(0, 0);
    screen_blank_line(0, 1);
    screen_line(0, 0, "SD card detected.", 0x3f);
    screen_line(0, 1, "reading DSP binary...", 0x3f);
    screen_refresh();

    // search for our dsp and load it
    // return success (0 == fail)
    if( files_search_dsp() ) {
      ;;
    } else {
      screen_clear();
      return 0;
    }

  } else {
    screen_blank_line(0, 0);
    screen_blank_line(0, 1);

    // firstrun pattern was set, so there should be a blackfin executable in flash.
    screen_line(0, 1, "loading flash to RAM...", 0x3f);
    screen_refresh();
    // read from flash to RAM
    flash_read_ldr();
    
    screen_line(0, 1, "booting DSP from flash...", 0x3f);
    screen_refresh();
    // reboot DSP from RAM
    bfin_load_buf();
  }

  screen_line(0, 1, "waiting for bfin init...      ", 0x3f);
  screen_refresh();

  // this is retarded, we need the GPIO for bfin to signal when init done
  delay_ms(1000);

    // report parameters

  screen_line(0, 1, "reporting bfin params...       ", 0x3f);
  screen_refresh();

  if( ctl_report_params() ) {
    ;;
  } else {
    //    screen_clear();
    
    screen_line(0, 1, "param report failed!           ", 0x3f);
    screen_refresh();
    return 0;
  }


    screen_line(0, 1, "setting initial parameters...  ", 0x3f);
    screen_refresh();

  ctl_init_params();

  delay_ms(20);

  // enable audio
  screen_line(0, 1, "run                                ", 0x3f);
  screen_refresh();

  bfin_enable();

  screen_clear();  
  screen_refresh();

  render_refresh();
  
  return 1;
}
