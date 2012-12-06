/* main.c
 * avr32_test
 * aleph
 *
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
#include "fix.h"
#include "simple_string.h"
#include "font.h"
#include "global.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "timers.h"


//=========================================
//==== static variables
//  flag to wait for startup button press
static u8 startup = 1;
// mode switch
static u8 mode = 0;

//=========================================== 
//==== static functions

//------  declare
//  app event loop
static void check_events(void);
// startup routine (perform this once after an initial button press)
static void perform_startup(void);

// get params from bfin
// static void report_params(void);


//----------------------------------------------
//---- testing stuff

// display scrolling history of events
static void scroll_event(const char* str, s16 val);
static char eventScroll[CHAR_ROWS][CHAR_COLS];
static u64 eventScrollTimes[CHAR_ROWS];
static s16 eventScrollData[CHAR_ROWS];
static u8 scrollIdx = 0;

// display ADC
static void displayAdcVal(u8 idx, u16 val);

// display processing time in event loop
// static void displayEventLoad(void);
// static u64 maxEventCycles = 0;
//-------------------------------------------------

// number of parameters
// volatile u32 numParams = 0;

//------- define
// app event loop
static void check_events(void) {
  static event_t e;  
  static u64 delay;
  // uiKey_t key;
  
  //static U64 cycles = 0;
  //static U64 cyclesNow = 0;

  //  key = eKeyDummy;
  
  //  cycles = Get_system_register(AVR32_COUNT)  

  if( get_next_event(&e) ) {
    //    print_dbg("\r\nevent!");

    /*
      if(startup) {
      /// a hack!
      /// this should select only switch events... hm
      //      print_dbg("\r\nstartup event: ");
      //      print_dbg_ulong(e.eventType);
      if(e.eventType > kEventEncoder3) { 
      //	perform_startup();
      //	return;
      }
      }
    */

    if(e.eventType > kEventRefresh) { 
      print_dbg("\r\n received UI event ");
      print_dbg_ulong(e.eventType);
    }
  

    switch(e.eventType) {
    case kEventSwitchDown0:
      // scroll_event(" sw f0 down", 0);
      // menu_handleKey(eKeyFnDownA);
      break;
    case kEventSwitchUp0:
      // scroll_event(" sw f0 up", 0);
      // menu_handleKey(eKeyFnUpA);
      break;
    case kEventSwitchDown1:
      // scroll_event(" sw f1 down", 0);
      // menu_handleKey(eKeyFnDownB);
      break;
    case kEventSwitchUp1:
      // scroll_event(" sw f1 up", 0);
      // menu_handleKey(eKeyFnUpB);
      break;
    case kEventSwitchDown2:
      // scroll_event(" sw f2 down", 0);
      // menu_handleKey(eKeyFnDownC);
      break;
    case kEventSwitchUp2:
      // scroll_event(" sw f2 up", 0);
      // menu_handleKey(eKeyFnUpC);
      break;
    case kEventSwitchDown3:
      // scroll_event(" sw f3 down", 0);
      // menu_handleKey(eKeyFnDownD);
      break;
    case kEventSwitchUp3:
      // scroll_event(" sw f3 up", 0);
      // menu_handleKey(eKeyFnUpD);
      break;
    case kEventSwitchDown4:
      mode ^= 1;
      if(mode) { gpio_set_gpio_pin(LED_MODE_PIN); }
      else { gpio_clr_gpio_pin(LED_MODE_PIN); }
      // scroll_event(" sw mode down", 0);
      break;
    case kEventSwitchUp4:
      // scroll_event(" sw mode up", 0);
      break;
    case kEventSwitchDown5:
      screen_line(0, 0, "powering down!", 0x3f);
      screen_refresh();
      delay = 500000; while(--delay) {;;}
      //      gpio_clr_gpio_pin(POWER_CTL_PIN);
      // scroll_event(" sw power down", 0);
      break;
    case kEventSwitchUp5:
      // scroll_event(" sw power up", 0);
      break;
    case kEventEncoder0:
      scroll_event(" encoder 0", e.eventData);
      if(e.eventData > 0) {
	// menu_handleKey(eKeyEncUpA);
      } else {
	// menu_handleKey(eKeyEncDownA);
      }
      break;
    case kEventEncoder1:
      scroll_event(" encoder 1", e.eventData);
      if(e.eventData > 0) {
	// menu_handleKey(eKeyEncUpB);
      } else {
	// menu_handleKey(eKeyEncDownB);
      }
      break;
    case kEventEncoder2:
      scroll_event(" encoder 2", e.eventData);
      if(e.eventData > 0) {
	// menu_handleKey(eKeyEncUpC);
      } else {
	// menu_handleKey(eKeyEncDownC);
      }
      break;
    case kEventEncoder3:
      scroll_event(" encoder 3", e.eventData);
      if(e.eventData > 0) {
	// menu_handleKey(eKeyEncUpD);
      } else {
	// menu_handleKey(eKeyEncDownD);
      }
      break;
    case kEventAdc0:
      //      print_dbg("\r\nadc val 0: ");
      //      print_dbg_ulong(e.eventData);
      displayAdcVal(0, e.eventData);
      break;
    case kEventAdc1:
      //      print_dbg("\r\nadc val 1: ");
      //      print_dbg_ulong(e.eventData);
      displayAdcVal(1, e.eventData);
      break;
    case kEventAdc2:
      //      print_dbg("\r\nadc val 2: ");
      //      print_dbg_ulong(e.eventData);
      displayAdcVal(2, e.eventData);
      break;
    case kEventAdc3:
      //      print_dbg("\r\nadc val 3: ");
      //      print_dbg_ulong(e.eventData);
      displayAdcVal(3, e.eventData);
      break;

    case kEventRefresh:
      //      print_dbg("\r\nrefresh!");
      //	if(refresh == 1) {
      screen_refresh();
      //	  refresh = 0;
      //	}
      break;
    }
    /*
      cyclesNow = Get_system_register(AVR32_COUNT);
      if(cyclesNow > maxEventCycles) {
      maxEventCycles = cyclesNow;
      //      displayEventLoad();
      }
    */
  } // if event
}


////main function
int main (void) {
  U32 waitForCard = 0;
  volatile avr32_tc_t *tc = APP_TC;
  //  volatile u64 delay;

  gpio_set_gpio_pin(POWER_CTL_PIN);
  
  // initialize clocks:
  init_clocks();
  
  // initialize Interrupt Controller
  INTC_init_interrupts();

  // disable interrupts
  Disable_global_interrupt();

  // initialize RS232 debug uart
  init_ftdi_usart();

  // initialize spi1: OLED, ADC, SD/MMC
  init_spi1();

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

  //memory manager
  init_mem();
  
  // Enable all interrupts.
  Enable_global_interrupt();

  // Wait for a card to be inserted
  screen_line(0, 0, "ALEPH", 0x3f);
  screen_line(0, 1, "waiting for SD card...", 0x3f);
  screen_refresh();
  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
  }
  screen_line(0, 2, "card detected.", 0x3f);
  refresh = 1;
      
  // set up file navigation, scan drives, list files
  init_files();
  //  files_list();  
  //  files_scan_dsp();

  // send ADC config
  init_adc();

  // start application timers
  init_app_timers();

  //  screen_line(0, 3, "press any key to continue...", 0x3f);
  refresh=1;

  
  //  print_dbg("starting event loop.\n\r");
  
  // event loop
  while(1) {
    //    check_debug_events();
    check_events();
  }
}

// startup BEES
void perform_startup(void) {
  
  startup = 0;
  screen_line(0, 0, "BEEES?", 0x3f);
  screen_blank_line(0, 1);
  screen_blank_line(0, 2);
  screen_blank_line(0, 3);
  screen_blank_line(0, 4);
  screen_blank_line(0, 5);
  screen_blank_line(0, 6);
  screen_blank_line(0, 7);

  print_dbg("\r\nTHE BEES.\r\n");
  
  //  net_init();
  //   menu_init();
  //  preset_init();
  //  scene_init();
}

/*

// load the default DSP
//load_bfin_sdcard_test();   
files_load_dsp("aleph-osc2.ldr");
// get parameters
print_dbg("\r\nwaiting to read params...");
//    delay_ms(1000);
delay = FCPU_HZ >> 8; while(delay--) {;;}
print_dbg("\r\nwaited.");
//
report_params();
// load scene 0
//...
*/

/// TEST: add a string to the scroll output

static void scroll_event(const char* str, s16 val) {
  /*
  u8 i;
  s8 n;
  u8 x=0;
  print_dbg("\r\n scrolling event: ");
  print_dbg(str);
  strcpy(eventScroll[scrollIdx], str);
  //  itoa_whole(val, eventScroll[scrollIdx] + CHAR_COLS - 6, 5); 
  eventScrollData[scrollIdx] = val;
  eventScrollTimes[scrollIdx] = tcTicks;
  // display
  for(i=0; i<CHAR_ROWS_1; i++) {
    n = scrollIdx - i;
    if(n < 0) { n += CHAR_ROWS; }
    screen_blank_line(0, i);
    screen_int(0, i, (s16)eventScrollTimes[n], 0xf);
    screen_line(60, i, eventScroll[n], 0xf);
    screen_int(CHAR_COLS - 5, i, eventScrollData[n], 0xf);
    screen_hl_line(x, i, n % 4);
  }
  // advance index
  scrollIdx = (scrollIdx + 1) % CHAR_ROWS;
  */
}



void displayAdcVal(u8 idx, u16 val) {
  screen_blank_line(0, CHAR_ROWS_1);
  screen_int(FONT_CHARW * (5 * idx + 3), CHAR_ROWS_1, val, 1);
  screen_hl_line(0, CHAR_ROWS_1, 0xf);
}

/*
  void displayEventLoad(void) {
  u8 x = 0;
  x = screen_line(0, 0, "max event ms: ", 0xf);
  screen_int(x, 0, (int)( (float)maxEventCycles / (float)FMCK_HZ * 1000.f ), 0xf );
  }
*/
