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
#include "delay.h"
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
#include "net.h"
#include "net_monome.h"
#include "pages.h"
#include "play.h"
#include "render.h"
#include "scene.h"

// this is called during hardware initialization.
// allocate memory.
void app_init(void) {
  print_dbg("\r\n net_init... ");
  net_init();

  print_dbg("\r\n preset_init...");  
  preset_init();

  print_dbg("\r\n scene_init...");
  scene_init();

  print_dbg("\r\n files_init...");
  files_init();

  /// WARNING: initialization order is important.

  print_dbg("\r\n render_init...");
  render_init();

  /// move these after scene load,
  // so that initial graphics reflect scene data
  /* print_dbg("\r\n pages_init..."); */
  /* pages_init(); */

  /* print_dbg("\r\n play_init..."); */
  /* play_init(); */
}

// this is called from main event handler
u8 app_launch(u8 firstrun) {

  print_dbg("\r\n launching app with firstrun: ");
  print_dbg_ulong(firstrun);

  //  net_print();

  if(firstrun) {
    print_dbg("\r\n first run, don't load DSP or scene");
    render_boot("launching app, first run");
  } else {
    print_dbg("\r\n booting default ldr from flash... ");
    render_boot("booting DSP from flash");
    flash_read_ldr();

    bfin_load_buf();    
    print_dbg("\r\n DSP booted, waiting to query params...");
    render_boot("waiting for DSP init");

    /// blackfin should clear ready pin ASAP on boot.
    /// but give it a moment to accomplish that.
    delay_ms(2);
    
    bfin_wait_ready();
    print_dbg(" requesting param report...");
    render_boot("requesting DSP params");
    net_report_params();

    print_dbg("\r\n enable DSP audio...");
    render_boot("enabling audio");
    bfin_enable();
    
    print_dbg("\r\n reading default scene... ");
    render_boot("reading default scene");
    scene_read_default();
  }

  // init pages (fill graphics buffers)
  print_dbg("\r\n pages_init...");
  pages_init();

  print_dbg("\r\n play_init...");
  play_init();

  // update page rendering and handlers
  pages_reselect();

  // enable timers
  print_dbg("\r\n enable app timers...");
  render_boot("enabling app timers");
  init_app_timers();
    
  // pull up power control pin, enabling soft-powerdown
  gpio_set_gpio_pin(POWER_CTL_PIN);

  // assign app event handlers
  print_dbg("\r\n assigning handlers ");
  assign_bees_event_handlers();

  return 1;
}
