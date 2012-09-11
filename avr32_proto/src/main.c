/* main,c
h * aleph-avr32
 *
 */


//// ASF
#include <string.h>
#include <stdio.h>
//#include <sysclk.h>
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "compiler.h"
#include "cycle_counter.h"
#include "ctrl_access.h"
#include "util.h"
#include "gpio.h"
#include "intc.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "sdramc.h"
#include "sysclk.h"
#include "usart.h"
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
#include "memory.h"
#include "timers.h"

// DEBUG: skip sdcard setuo
// #define SKIPSD 1

//=========================================
//==== static variables
static u8 initFlag = 1;

//=========================================== 
//==== static functions

//------  declare
// debug events
static void check_debug_events(void);
//  app event loop
static void check_events(void);
// wait for first UI event after startup
static U8 check_init(void);
// get params from bfin
static void report_params(void);
// number of parameters
volatile u32 numParams = 0;

//------- define
// app event loop
static void check_events(void) {
  static event_t e;  
  uiKey_t key;
  
  //static U64 cycles = 0;
  //static U64 cyclesNow = 0;

  key = eKeyDummy;
  
  if( get_next_event(&e) ) {

    switch(e.eventType) {
      
    case kEventEncoder0:
      if(e.eventData > 0) {
	key = eKeyEncUpA;
      } else {
	key = eKeyEncDownA;
      }
      break;

    case kEventEncoder1:
      if(e.eventData > 0) {
	key = eKeyEncUpB;
      } else {
	key = eKeyEncDownB;
      }
      break;

    case kEventEncoder2:
      if(e.eventData > 0) {
	key = eKeyEncUpC;
      } else {
	key = eKeyEncDownC;
      }
      break;

    case kEventEncoder3:
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
      key = eKeyFnUpA;
      break;

    case kEventSwitchDown1:
      if (check_init()) { return; }
      key = eKeyFnDownB;
      break;
    case kEventSwitchUp1:
      key = eKeyFnUpB;
      break;
      
    case kEventSwitchDown2:
      if (check_init()) { return; }
      key = eKeyFnDownC;
      break;
    case kEventSwitchUp2:
      key = eKeyFnUpC;
      break;
      
    case kEventSwitchDown3:
      if (check_init()) { return; }
      key = eKeyFnDownD;
      break;
    case kEventSwitchUp3:
      key = eKeyFnUpD;
      break;

    case kEventAdc0:
      screen_int(0, FONT_CHARH * (NROWS - 2), e.eventData, 0xf);
      refresh = 1;
      break;

    case kEventRefresh:
      if(refresh == 1) {
	screen_refresh();

	refresh = 0;
      }
      break;
    } // switch event

    if(key != eKeyDummy) { 
      // cycles = Get_system_register(AVR32_COUNT);

      ////// menu_handleKey(key); 
      // test:
      /*
      switch(key) {
      case eKeyFnDownA:
	
	break;
      case eKeyFnDownB:
	break;
      case eKeyFnDownC:
	break;
      case eKeyFnDownD:
	break;
      default:
	
      }
*/
      

      
      // cyclesNow = Get_system_register(AVR32_COUNT);
      // print_dbg(" event:"); print_dbg_ulong(e.eventType);
      // print_dbg(" cycles:"); print_dbg_ulong(cyclesNow - cycles);
      refresh = 1;
    }
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

  // initialize sdram
  sdramc_init(FMCK_HZ);
  //memory managaer
  init_mem();

  // Enable all interrupts.
  Enable_global_interrupt();

  print_dbg("\r\nALEPH\r\n ");

#ifndef SKIPSD
  // Wait for a card to be inserted
  //  print_dbg("\r\nwaiting for SD card... ");

  screen_line(0, 0, "waiting for SD card...", 2); refresh=1;
  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
  }
  screen_line(0, 0, "card detected.", 2); refresh=1;

  // set up file navigation using available drives
  init_files();

  // list files to USART
  files_list();

#endif 

  // send ADC config
  init_adc();
  init_app_timers();

  print_dbg("starting event loop.\n\r");
  screen_line(0, 0, "ALEPH hardware initialized.", 2); 
  screen_line(0, FONT_CHARH, "press FN1 key to begin BEES.", 2); refresh=1;

  // event loop
    while(1) {
      check_debug_events();
      check_events();
    }
}

// wait for a button press to initialize BEES
U8 check_init(void) {
  volatile u64 delay;
  if(initFlag) {
    // initialize BEES components
    net_init();
    menu_init();
    preset_init();
    scene_init();

#ifndef SKIPSD
    // load the default DSP
    //load_bfin_sdcard_test();   
    files_load_dsp_name("aleph-osc2.ldr");
    // get parameters
    print_dbg("\r\nwaiting to read params...");
    //    delay_ms(1000);
    delay = FCPU_HZ >> 8; while(delay--) {;;}
    print_dbg("\r\nwaited.");
    //
    report_params();
    // load scene 0
    //...
#endif

    refresh = 1;
    initFlag = 0;
    return 1;
  } else {
    return 0;
  }
}

static void report_params(void) {
  volatile ParamDesc pdesc;
  u8 i;

  bfin_get_num_params(&numParams);
  print_dbg("\r\nnumparams: ");
  print_dbg_ulong(numParams);

  if(numParams > 0) {
    net_clear_params();
    for(i=0; i<numParams; i++) {
      bfin_get_param_desc(i, &pdesc);
      net_add_param(i, &pdesc);
    }
  }
}

void check_debug_events(void) {
  static event_t e;
  static int c;

  e.eventType = kNumEvents;

  if(usart_test_hit(DBG_USART)) {
    usart_read_char(DBG_USART, &c);
    
        print_dbg("\r\ndbg: ");
        print_dbg_ulong(c);

    switch(c) {
    case '-':
      e.eventType = kEventEncoder0;
      e.eventData = -1;
      break;
    case '=':
      e.eventType = kEventEncoder0;
      e.eventData = 1;
      break;
    case '[':
      e.eventType = kEventEncoder1;
      e.eventData = -1;
      break;
    case ']':
      e.eventType = kEventEncoder1;
      e.eventData = 1;
      break;
    case ';':
      e.eventType = kEventEncoder2;
      e.eventData = -1;
      break;
    case '\'':
      e.eventType = kEventEncoder2;
      e.eventData = 1;
      break;
    case '.':
      e.eventType = kEventEncoder3;
      e.eventData = -1;
      break;
    case '/':
      e.eventType = kEventEncoder3;
      e.eventData = 1;
      break;
    case 'z':
      e.eventType = kEventSwitchDown0;
      break;
    case 'x':
      e.eventType = kEventSwitchDown1;
      break;
    case 'c':
      e.eventType = kEventSwitchDown2;
      break;
    case 'v':
      e.eventType = kEventSwitchDown3;
      break;
    }

    if(e.eventType != kNumEvents) {
      post_event(&e);
    }
  }
}
