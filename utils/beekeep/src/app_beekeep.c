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
#include "op_pool.h"
#include "op.h"
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

#include <stdio.h>
#include "net.h"
void print_net () {
  int i;
  for (i=0; i < net->numOps; i++) {
    printf("printing op %d (%s)\n", i, net_op_name(i));
  }
  for(i=0; i < net->numIns; i++) {
    printf("printing in %d (%s) from op %d, %d\n",
	   i, net_in_name(i), net->ins[i].opIdx, net->ins[i].opInIdx);
  }
  for(i=0; i < net->numOuts; i++) {
    printf("printing out %d (%s) from op %d, %d\n",
	   i, net_out_name(i), net->outs[i].opIdx, net->outs[i].opOutIdx);
  }
}

void test_net (void) {
  print_net();
  net_add_op_at(12, net->numOps);
  net_add_op_at(13, net->numOps-1);
  /* net_add_op_at(12, net->numOps); */
  /* net_add_op_at(12, net->numOps); */
  /* net_add_op_at(12, net->numOps); */
  print_net();
}
void t1_callback(void *foo) {
  printf("\r\nt1_callback");
}
void t2_callback(void *foo) {
  printf("\r\nt2_callback");
}

void beekeep_test_timers(void) {
  softTimer_t t1;
  softTimer_t t2;
  timer_add(&t1, 10, &t1_callback, NULL);
  timer_add(&t1, 10, &t1_callback, NULL);
  timer_remove(&t1);
  timer_add(&t1, 10, &t1_callback, NULL);
  timer_add(&t2, 20, &t2_callback, NULL);

  int i;
  for(i=0; i < 50; i++) {
    process_timers();
  }
  timer_remove(&t1);
  timer_remove(&t2);
}

#include "scalers/scaler_label.h"

void beekeep_print_label (void) {
  int i;
  char str[256];
  for(i=0; i < scaler_n_labels; i++) {
    scaler_labels_str(str, NULL, i);
    printf("\r\nscaler %d: %s", i, str);
  }
}

void beekeep_dummy_load_label (void) {
  scaler_start_parse_labels ();

  FILE *f;
  f = fopen("dummy_patchlabel.lab", "r");
  char c;
  while ((c = getc(f)) != EOF) {
    scaler_parse_labels_char(c);
  }
  fclose(f);
}

// this is called during hardware initialization.
// allocate memory.
void app_init(void) {
  //  beekeep_test_timers();
  int i;
  for(i=0; i < numOpClasses; i++) {
    printf("size of %s: %d\n", op_registry[i].name, op_registry[i].size);
  }
  initBigMemPool();
  initSmallMemPool();
  ParamDesc pd = {.type = eParamTypeLabel};
  ParamScaler ps;
  scaler_init(&ps, &pd);
  scaler_labels_init(&ps);

  // first print off the default label config
  /* printf("\r\ndefault label:"); */
  /* beekeep_print_label(); */

  // now test out the code to load new patch labels (char-by-char)
  /* printf("\r\nexample patch matrix from .ldr:"); */
  /* beekeep_dummy_load_label(); */
  /* beekeep_print_label(); */
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

  /* test_net(); */

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
