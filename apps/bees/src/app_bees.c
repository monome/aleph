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

// libavr32
#include "events.h"
#include "monome.h"
#include "screen.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "flash.h"

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

// maj = 1byte, min = 1byte, rev = 2byte
//const u32 beesVersion = (MAJ << 24) | (MIN << 16) | (REV << 8);
const AppVersion beesVersion = { .min = MIN , .maj = MAJ , .rev = REV };

//--------------------------------
//--- static vars
static char versionString[12] = VERSIONSTRING;

#define DEFAULT_LDR "waves.ldr"
#define DEFAULT_DSC "waves.dsc"

// this is called during hardware initialization.
// allocate memory.
void app_init(void) {

  print_dbg("\r\n preset_init...");  
  presets_init();

  // this must come after preset init!
  // uses preset data when adding system ops...
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
u8 app_launch(eLaunchState state) {

  print_dbg("\r\n launching app with state: ");
  print_dbg_ulong(state);

  //  net_print();
  
  render_boot("BEES");
  render_boot(versionString);

  while (!sd_mmc_spi_mem_check()) {
    render_boot("waiting for SD card...");
  }

  if(state == eLaunchStateFirstRun) {
    render_boot("launching app, first run");
    print_dbg("\r\n first run, writing nonvolatile data...");
    
    ///... write param scaler data
    // this is done at firstrun instead of being linked statically,
    // so that users can tune scaler data offline without recompiling
    render_boot("init param scaling data...");
    flash_init_scaler_data();

    print_dbg("\r\n first run, try and load default DSP");
    render_boot("launching default DSP");

    //// startup using default DSP name
    files_load_dsp_name(DEFAULT_LDR);
    
    render_boot("waiting for DSP init...");
    bfin_wait_ready();

    //    print_dbg("\r\n enable DSP audio...");
    render_boot("enabling audio");
    bfin_enable();
    
  } else {

    app_pause();

    /// blackfin should clear ready pin ASAP on boot.
    /// but give it a moment to accomplish that.
    delay_ms(2);

    /// read the default scene from sd card
    /// this also attempts to load associated .ldr    

    if(state == eLaunchStateClean) {
      render_boot("reading clean-boot scene");
      print_dbg("\r\n loading clean-boot scene. current module name from sceneData: ");
      print_dbg(sceneData->desc.moduleName);
      scene_read_clean();
    } else {
      render_boot("reading default scene");
      print_dbg("\r\n loading default scene. current module name from sceneData: ");
      print_dbg(sceneData->desc.moduleName);
      scene_read_default();
    }

    delay_ms(2); 

    app_resume();
    
   }

  // init pages (fill graphics buffers)
  render_boot("initializing gfx");
  print_dbg("\r\n pages_init...");
  pages_init();

  print_dbg("\r\n play_init...");
  play_init();

  // enable timers
  print_dbg("\r\n enable app timers...");
  render_boot("enabling app timers");
  init_app_timers();

  // pull up power control pin, enabling soft-powerdown
  //  gpio_set_gpio_pin(POWER_CTL_PIN);

  // assign app event handlers
  print_dbg("\r\n assigning handlers... ");
  render_boot("assigning UI handlers");
  assign_bees_event_handlers();

  // update page rendering and handlers...
  pages_reselect();

  // start in play mode if not firstrun 
  if(state != eLaunchStateFirstRun) pages_toggle_play();

  return 1;
}
