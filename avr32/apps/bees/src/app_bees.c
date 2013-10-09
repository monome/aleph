/*
  app_bees.c
  aleph-avr32
  bees

  required app-specific implementation of avr32/src/app.h
 */

// std
/* #include <string.h> */
/* #include <stdio.h> */

// asf
#include "gpio.h"
#include "print_funcs.h"

// aleph-avr32nothing seems 
#include "app.h"
#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "flash.h"
#include "monome.h"
#include "screen.h"
// bees
#include "files.h"
#include "handler.h"
#include "menu.h"
#include "net_monome.h"
#include "play.h"
#include "scene.h"


// this is called during hardware initialization.
// use for memory allocation.
void app_init(void) {
  net_init();
  print_dbg("\r\n net_init");
  
  preset_init();
  print_dbg("\r\n preset_init");

  scene_init();
  print_dbg("\r\n scene_init");

  files_init();
  print_dbg("\r\n files_init");

  menu_init();
  print_dbg("\r\n menu_init");

  play_init();
  print_dbg("\r\n play_init");

  // set handler
  appEventHandler = &bees_handler;
  //  render_init();
}

// this is called from the event queue 
u8 app_launch(u8 firstrun) {
  ////////////////
  /* ///// TEST */
  /* char buf[64]; */
  /* sprintf(buf, " blah blah oh yeah , %d", 666); */

  /////////////////


  //  app_notify("launching BEES");
  print_dbg("\r\n launching app with firstrun: ");
  print_dbg_ulong(firstrun);
  if(firstrun) {
    scene_write_default();
  } else {
    print_dbg("\r\n booting default ldr from flash... ");

    flash_read_ldr();
    bfin_load_buf();

    scene_read_default();
  }
  //menu_refresh();

  return 1;
}
