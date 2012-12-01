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
static u8 initFlag = 1;

//=========================================== 
//==== static functions

//------  declare
//  app event loop
static void check_events(void);
// wait for first UI event after startup
static U8 check_init(void);
// get params from bfinx
static void report_params(void);
// check for debug event commands
static char check_debug_events(void);

// display scrolling history of events
static void scroll_event(const char* str, s16 val);
static char eventScroll[CHAR_ROWS][CHAR_COLS];
static u64 eventScrollTimes[CHAR_ROWS];
static s16 eventScrollData[CHAR_ROWS];
static u8 scrollIdx = 0;
// display ADC
static void displayAdcVal(u8 idx, u16 val);
// display processing time in event loop
static void displayEventLoad(void);
static u64 maxEventCycles = 0;
// display hmatrix SFR contents
//static void show_hmatrix_sfr(void);
// display SDRAM config
static void show_sdram_constants(void);

static u8 ramTestFlag = 0;

// number of parameters
volatile u32 numParams = 0;

//------- define
// app event loop
static void check_events(void) {
  static event_t e;  
  uiKey_t key;
  
  static U64 cycles = 0;
  static U64 cyclesNow = 0;

  key = eKeyDummy;
  
  cycles = Get_system_register(AVR32_COUNT);

  if( get_next_event(&e) ) {

    switch(e.eventType) {
    case kEventSwitchDown0:
      scroll_event(" sw f0 down", 0);
      break;
    case kEventSwitchUp0:
      scroll_event(" sw f0 up", 0);
      break;
    case kEventSwitchDown1:
      scroll_event(" sw f1 down", 0);
      break;
    case kEventSwitchUp1:
      scroll_event(" sw f1 up", 0);
      break;
    case kEventSwitchDown2:
      scroll_event(" sw f2 down", 0);
      break;
    case kEventSwitchUp2:
      scroll_event(" sw f2 up", 0);
      break;
    case kEventSwitchDown3:
      scroll_event(" sw f3 down", 0);
      break;
    case kEventSwitchUp3:
      scroll_event(" sw f3 up", 0);
      break;
    case kEventSwitchDown4:
      scroll_event(" sw edit down", 0);
      break;
    case kEventSwitchUp4:
      scroll_event(" sw edit up", 0);
      break;
    case kEventEncoder0:
      scroll_event(" encoder 0", e.eventData);
      break;
    case kEventEncoder1:
      scroll_event(" encoder 1", e.eventData);
      break;
    case kEventEncoder2:
      scroll_event(" encoder 2", e.eventData);
      break;
    case kEventEncoder3:
      scroll_event(" encoder 3", e.eventData);
      break;
    case kEventAdc0:
      print_dbg("\r\nadc val 0: ");
      print_dbg_ulong(e.eventData);
      displayAdcVal(0, e.eventData);
      break;
    case kEventAdc1:
      print_dbg("\r\nadc val 1: ");
      print_dbg_ulong(e.eventData);
      displayAdcVal(1, e.eventData);
      break;
    case kEventAdc2:
      print_dbg("\r\nadc val 2: ");
      print_dbg_ulong(e.eventData);
      displayAdcVal(2, e.eventData);
      break;
    case kEventAdc3:
      print_dbg("\r\nadc val 3: ");
      print_dbg_ulong(e.eventData);
      displayAdcVal(3, e.eventData);
      break;

    case kEventRefresh:
      if(refresh == 1) {
	screen_refresh();
	refresh = 0;
      }
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
  volatile u64 delay;
  
  // switch to osc0 for main clock
  //   pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 
  // initialize clocks:
  init_clocks();
  
  // initialize Interrupt Controller
  INTC_init_interrupts();

  // disable interrupts
  Disable_global_interrupt();

  // initialize RS232 debug uart
  init_ftdi_usart();

  // initialize oled uart in SPI mode
  init_oled_spi();

  // initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
  //  init_sd_mmc_resources();

  // initialize PDCA controller
  //  init_local_pdca();

  // initialize blackfin resources
  //  init_bfin_resources();

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

  //  delay = 10000; while(delay-- > 0) { ;; } 

  screen_test_fill();

  /*
  print_dbg("\r\nALEPH\r\n ");

  ////////////////////
 // Wait for a card to be inserted
  //  print_dbg("\r\nwaiting for SD card... ");

  screen_line(0, 0, "waiting for SD card...", 2); screen_refresh();
  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
  }
  screen_line(0, 0, "card detected.", 2); refresh=1;
  */

  /*
  // set up file navigation using available drives
  init_files();

  // list files to USART
  files_list();
  
  screen_refresh();
  */

  // send ADC config
  //  init_adc();

  // start application timers
  init_app_timers();
  
  print_dbg("starting event loop.\n\r");
  
  // event loop
  while(1) {
    //    check_debug_events();
    check_events();
  }
}

/*
// wait for a button press to initialize BEES
U8 check_init(void) {
  volatile u64 delay;
  if(initFlag) {
    // initialize BEES components
    net_init();
    menu_init();
    preset_init();
    scene_init();

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


    refresh = 1;
    initFlag = 0;
    return 1;
  } else {
    return 0;
  }
}
*/

// check the debug command 
char check_debug_events(void) {
  return 1;
}


/// TEST: add a string to the scroll output
static void scroll_event(const char* str, s16 val) {
  u8 i;
  s8 n;
  u8 x=0;
  // timestamp
  //  itoa_whole((int)tcTicks, eventScroll[scrollIdx], 6);
  // string
  //  str_copy(str, eventScroll[scrollIdx], 10);
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
    screen_int(0, i, (s16)eventScrollTimes[n], 0xf);
    screen_line(50, i, eventScroll[n], 0xf);
    screen_int(CHAR_COLS - 5, i, eventScrollData[n], 0xf);
    screen_hl_line(x, FONT_CHARH * i, n % 2);
  }
  // advance index
  scrollIdx = (scrollIdx + 1) % CHAR_ROWS;
}

void displayAdcVal(u8 idx, u16 val) {
  screen_int(FONT_CHARW * (5 * idx + 3), CHAR_ROWS_1 * FONT_CHARH, val, 1);
  screen_hl_line(0, CHAR_ROWS_1 * FONT_CHARH, 0xf);
}

void displayEventLoad(void) {
  u8 x = 0;
  x = screen_line(0, 0, "max event ms: ", 0xf);
  screen_int(x, 0, (int)( (float)maxEventCycles / (float)FMCK_HZ * 1000.f ), 0xf );
}
