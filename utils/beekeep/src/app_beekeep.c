/*
  app_bees.c
  aleph-avr32
  bees

  required app-specific implementation of avr32/src/app.h
 */

// asf
#include "delay.h"
// #include "gpio.h"
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
#include "app_bees.h"
#include "app_timers.h"
#include "files.h"
#include "flash_bees.h"
#include "handler.h"
#include "net.h"
#include "net_monome.h"
#include "pages.h"
#include "play.h"
#include "render.h"
#include "scene.h"

//-------------------------------------------
//-- extern vars (here)


#define MIN 0
#define MAJ 0
#define REV 0
#define VERSIONSTRING "none"

// maj = 1byte, min = 1byte, rev = 2byte
//const u32 beesVersion = (MAJ << 24) | (MIN << 16) | (REV << 8);
const AppVersion beesVersion = { .min = MIN , .maj = MAJ , .rev = REV };

//--------------------------------
//--- static vars
static char versionString[12] = VERSIONSTRING;

// this is called during hardware initialization.
// allocate memory.
void app_init(void) {

  //hm
  print_dbg("\r\n preset_init...");  
  presets_init();

  print_dbg("\r\n net_init... ");
  net_init();

  print_dbg("\r\n scene_init...");
  scene_init();

  print_dbg("\r\n files_init...");
  files_init();

  /// WARNING: initialization order is important.

  print_dbg("\r\n render_init...");
  render_init();

  // initialize flash-management buffers
  print_dbg("\r\n flash_bees_init...");
  flash_bees_init();
}

// this is called from main event handler
u8 app_launch(u8 firstrun) {


  print_dbg("\r\n launching app with firstrun: ");
  print_dbg_ulong(firstrun);

  return 1;

  /* //  net_print(); */
  
  /* render_boot("BEES"); */
  /* render_boot(versionString); */

  /* if(firstrun) { */
  /*   render_boot("launching app, first run"); */
  /*   print_dbg("\r\n first run, writing nonvolatile data..."); */
    
  /*   ///... write param scaler data */
  /*   // this is done at firstrun instead of being linked statically, */
  /*   // so that users can tune scaler data offline without recompiling */
  /*   render_boot("init param scaling data..."); */
  /*   flash_init_scaler_data(); */

  /*   print_dbg("\r\n first run, try and load default DSP"); */
  /*   render_boot("launching default DSP..."); */

  /*   files_load_dsp_name("aleph-waves.ldr"); */
    
  /*   render_boot("waiting for DSP init..."); */
  /*   print_dbg("\r\n DSP booted, waiting to query params..."); */
  /*   print_dbg(" requesting param report..."); */
  /*   bfin_wait_ready(); */

  /*   //    print_dbg(" requesting param report..."); */
  /*   render_boot("requesting DSP params"); */
  /*   net_report_params(); */

  /*   //    print_dbg("\r\n enable DSP audio..."); */
  /*   render_boot("enabling audio"); */
  /*   bfin_enable(); */

  /*   //    render_boot("writing default dsp to flash..."); */
  /*   //    files_store_default_dsp_name("aleph-waves.ldr"); */
    
  /* } else { */

  /*   app_pause(); */

  /*   //    print_dbg("\r\n booting default ldr from flash... "); */
  /*   //    render_boot("booting DSP from flash"); */
  /*   //    flash_read_ldr(); */

  /*   //    bfin_load_buf();     */
  /*   //    print_dbg("\r\n DSP booted, waiting to query params..."); */
  /*   //    render_boot("waiting for DSP init..."); */

  /*   /// blackfin should clear ready pin ASAP on boot. */
  /*   /// but give it a moment to accomplish that. */
  /*   delay_ms(2); */
    
  /*   //    bfin_wait_ready(); */
  /*   //    print_dbg(" requesting param report..."); */
  /*   //    render_boot("requesting DSP params"); */
  /*   //    net_report_params(); */

  /*   //    print_dbg("\r\n enable DSP audio..."); */
  /*   //    render_boot("enabling audio"); */
  /*   //    bfin_enable(); */
    
  /*   print_dbg("\r\n reading default scene... "); */
  /*   render_boot("reading default scene"); */

  /*   /// this also attempts to load associated .ldr */
  /*   scene_read_default(); */

  /*   delay_ms(2);  */

  /*   app_resume(); */
    
  /*  } */

  /* // init pages (fill graphics buffers) */
  /* print_dbg("\r\n pages_init..."); */
  /* pages_init(); */

  /* print_dbg("\r\n play_init..."); */
  /* play_init(); */

  /* // enable timers */
  /* print_dbg("\r\n enable app timers..."); */
  /* render_boot("enabling app timers..."); */
  /* init_app_timers(); */

  /* // pull up power control pin, enabling soft-powerdown */
  /* //  gpio_set_gpio_pin(POWER_CTL_PIN); */

  /* // assign app event handlers */
  /* print_dbg("\r\n assigning handlers "); */
  /* render_boot("assigning UI handlers..."); */
  /* assign_bees_event_handlers(); */

  /* // update page rendering and handlers */
  /* pages_reselect(); */

  /* // start in play mode  */
  /* pages_toggle_play(); */

  /* return 1; */
}
