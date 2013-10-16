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

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "flash.h"
#include "monome.h"
#include "screen.h"
// bees
#include "app_timers.h"
#include "files.h"
#include "handler.h"
#include "menu.h"
#include "net.h"
#include "net_monome.h"
#include "play.h"
#include "render.h"
#include "scene.h"


// this is called during hardware initialization.
// use for memory allocation.
void app_init(void) {
  print_dbg("\r\n net_... ");
  net_init();

  print_dbg("\r\n preset_init...");  
  preset_init();

  print_dbg("\r\n scene_init...");
  scene_init();

  print_dbg("\r\n files_init...");
  files_init();

  print_dbg("\r\n render_init...");
  render_init();

  print_dbg("\r\n menu_init...");
  menu_init();

  print_dbg("\r\n play_init...");
  //  print_dbg(" ( not really )");
  play_init();

  // set handler
  print_dbg("\r\n setting handler ");
  appEventHandler = &bees_handler;
}

// this is called from the event queue 
u8 app_launch(u8 firstrun) {


  print_dbg("\r\n launching app with firstrun: ");
  print_dbg_ulong(firstrun);

  net_print();

  if(firstrun) {
    //    scene_write_default();
  } else {
    print_dbg("\r\n booting default ldr from flash... ");
    print_dbg(" ( not really ) ");
    //    flash_read_ldr();
    //    bfin_load_buf();
    

    print_dbg("\r\n enable DSP audio...");
    bfin_enable();

    // enable timers
    print_dbg("\r\n enable app timers...");
    init_app_timers();
    
    print_dbg("\r\n reading default scene... ");
    print_dbg("( not really )");
    //    scene_read_default();
  }
  //menu_refresh();

  return 1;
}
