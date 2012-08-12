/* main,c
 * aleph-avr32
 *
 */


//// ASF
#include <string.h>
#include <stdio.h>
//#include <sysclk.h>
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "compiler.h"
#include "ctrl_access.h"
#include "util.h"
#include "gpio.h"
#include "intc.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "sysclk.h"
//// aleph
// bees
#include "menu.h"
#include "net.h"
#include "preset.h"
#include "scene.h"
// common
#include "files.h"
#include "param_common.h"
#include "screen.h"
#include "types.h"
// avr32
#include "adc.h"
#include "app_timers.h"
#include "bfin.h"
#include "conf_aleph.h"
#include "encoders.h"
#include "events.h"
#include "font.h"
#include "global.h"
#include "init.h"
#include "interrupts.h"
#include "timers.h"

//=========================================
//==== static variables
static u8 initFlag = 1;

//=========================================== 
//==== static functions

//------  declare
//  app event loop
static void check_events(void);
// wait for first UI event after startup
static U8 check_init(void);
// get params from bfin
static void report_params(void);

//------- define
// app event loop
static void check_events(void) {
  static uiKey_t key = eKeyDummy;
  static event_t e;
  
  if( get_next_event(&e) ) {
    switch(e.eventType) {
      
    case kEventEncoder0:
      if (check_init()) { return; }
      if(e.eventData > 0) {
	key = eKeyEncUpA;
      } else {
	key = eKeyEncDownA;
      }
      break;

    case kEventEncoder1:
      if (check_init()) { return; }
      if(e.eventData > 0) {
	key = eKeyEncUpB;
      } else {
	key = eKeyEncDownB;
      }
      break;

    case kEventEncoder2:
      if (check_init()) { return; }
      if(e.eventData > 0) {
	key = eKeyEncUpC;
      } else {
	key = eKeyEncDownC;
      }
      break;

    case kEventEncoder3:
      if (check_init()) { return; }
      if(e.eventData > 0) {
	key = eKeyEncUpD;
      } else {
	key = eKeyEncDownD;
      }
      break;

    case kEventSwitchDown0:
      if (check_init()) { return; }
      key = eKeyFnDownA;
      break;
    case kEventSwitchUp0:
      if (check_init()) { return; }
      key = eKeyFnUpA;
      break;

    case kEventSwitchDown1:
      if (check_init()) { return; }
      key = eKeyFnDownB;
      break;
    case kEventSwitchUp1:
      if (check_init()) { return; }
      key = eKeyFnUpB;
      break;
      
    case kEventSwitchDown2:
      if (check_init()) { return; }
      key = eKeyFnDownC;
      break;
    case kEventSwitchUp2:
      if (check_init()) { return; }
      key = eKeyFnUpC;
      break;
      
    case kEventSwitchDown3:
      if (check_init()) { return; }
      key = eKeyFnDownD;
      break;
    case kEventSwitchUp3:
      if (check_init()) { return; }
      key = eKeyFnUpD;
      break;

    case kEventAdc0:
      print_dbg("\r\n got adc0 event: ");
      print_dbg_ulong(e.eventData);
      screen_int(0, FONT_CHARH * (NROWS - 2), e.eventData, 0xf);
      refresh = 1;
      break;

    case kEventRefresh:
      screen_refresh();
      refresh = 0;
      break;


    if(key != eKeyDummy) { menu_handleKey(key); }

    } // switch event
  } // if event
}


////main function
int main (void) {
  U32 waitForCard = 0;
  volatile avr32_tc_t *tc = APP_TC;
  
  // switch to osc0 for main clock
  //  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 
  // initialize clocks:
  init_clocks();
  
  // initialize Interrupt Controllvoider
  INTC_init_interrupts();

  // disable interrupts
  Disable_global_interrupt();

  // initialize RS232 debug uart
  init_dbg_usart();

  // initialize oled uart in SPI mode
  init_oled_usart();

  // initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
  init_sd_mmc_resources();

  // initialize PDCA controller
  init_local_pdca();

  // initialize blackfin resources
  init_bfin_resources();

  // initialize application timer
  init_tc(tc);

  // initialize other GPIO
  init_gpio();

  // register interrupts
  register_interrupts();

  // initialize the OLED screen
  init_oled();
  
  // intialize the event queue
  init_events();
  
  // intialize encoders
  init_encoders();

  // Enable all interrupts.
  Enable_global_interrupt();

  print_dbg("\r\nALEPH\r\n ");

  // Wait for a card to be inserted
  print_dbg("\r\nwaiting for SD card... ");

  screen_string(0, 0, "waiting for card...", 2); refresh=1;
  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
  }
  screen_string(0, 0, "card detected.", 2); refresh=1;

  // set up file navigation using available drives
  init_files();

  // list files to USART
  files_list();
  
  // load blackfin from first .ldr file in filesystem
  load_bfin_sdcard_test();

  print_dbg("loaded dsp program.\n\r");

  // send ADC config
  init_adc();

  init_app_timers();

  print_dbg("starting event loop.\n\r");
  screen_string(0, 0, "ALEPH hardware initialized.", 2); 
  screen_string(0, 0, "use any key to begin BEES.", 2); refresh=1;
  
// event loop
    while(1) {
      check_events();
    }
}

// wait for a button press to initialize 
U8 check_init(void) {
  if(initFlag) {

    // initialize BEES components
    net_init();
    menu_init();
    preset_init();
    scene_init();

    // get default parameters
    report_params();

    // load scene 0
   
    initFlag = 0;
    return 1;
  } else {
    return 0;
  }
}

static void report_params(void) {
  ParamDesc pdesc;
  u8 np, i;

  np = bfin_get_num_params();
  if(np > 0) {
    net_clear_params();
    for(i=0; i<np; i++) {
      bfin_get_param_desc(i, &pdesc);
      net_add_param(i, &pdesc);
    }
  }
}

    /*
static void report_params(void) {
  //U8 col;

  U8 i;

  print_dbg("\n\requesting parameters..."); 
  numParams =  bfin_get_num_params();
  print_dbg("done.\n\r");

  for(i=0; i<numParams; i++) {
    bfin_get_param_desc(i,&( paramDesc[i]));
    paramVal[i].asFloat = paramDesc[i].min;
  }
  refresh_params();

  
    print_dbg(desc.label);
    print_dbg("\r\n");
    print_dbg(desc.unit);
    print_dbg("\r\n");
  
}

static void refresh_params(void) {
  u8 i, col, line;
  for(i=0; i<numParams; i++) {
    line = FONT_CHARH * (i);
    //screen_blank_line(0, line);
    col = screen_string_sqapplicationvoid ueeze(0, line, paramDesc[i].label, 0xf);
    col++;
    col = screen_string_squeeze(col, line, " : ", 0xf);
    col++;
    col = screen_float(col, line, paramVal[i].asFloat, 0xf);
    col++;
    col = screen_string_squeeze(col, line, " ", 0xf);
    col++;
    col = screen_string_squeeze(col, line, paramDesc[i].unit, 0x0f);
    //if(i == sel) { screen_hilite_line(0, line, 0x01); }
    refresh = 1;
  }
}
*/
