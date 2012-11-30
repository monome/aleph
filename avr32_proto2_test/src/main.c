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
    cyclesNow = Get_system_register(AVR32_COUNT);
    if(cyclesNow > maxEventCycles) {
      maxEventCycles = cyclesNow;
      //      displayEventLoad();
    }
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
  //  init_encoders();

  // initialize sdram
     sdramc_init(FMCK_HZ);
  //    sdramc_init(FOSC0);
  //memory manager
  //init_mem();
  
  // Enable all interrupts.
  Enable_global_interrupt();

  //  delay = 10000; while(delay-- > 0) { ;; } 

  //  screen_test_fill();

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

  // set up file navigation using available drives
  //  init_files();

  // list files to USART
  //  files_list();
  // test sdram
  //  screen_line(0, (CHAR_ROWS-2) * FONT_CHARH, "testing sdram...", 0xf); screen_refresh();
  
  //  show_hmatrix_sfr();
  show_sdram_constants();
  sdram_test(1);
  screen_refresh();
  ////////////////////////

  // send ADC config
  //  init_adc();
  /////// TEST:
  //  screen_line(0, CHAR_ROWS_1 * FONT_CHARH, "ADC: ", 0x1);
  // first 2 adc channels with pullup (switch mode),
  //  gpio_set_gpio_pin(AUX_PULLUP0_PIN);
  //  gpio_set_gpio_pin(AUX_PULLUP1_PIN);
  // second 2 wihtout pullup (cv/expr mode)
  //  gpio_clr_gpio_pin(AUX_PULLUP2_PIN);
  //  gpio_clr_gpio_pin(AUX_PULLUP3_PIN);
  ////////

  // start application timers
  //  init_app_timers();

  print_dbg("starting event loop.\n\r");
  
  // event loop
  while(1) {
    //    check_debug_events();
        check_events();
    //  sdram_test(0);
    //delay = 10000; while(delay > 0) { delay--; }
    //screen_refresh();
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
    x = screen_int(0, FONT_CHARH * i, (s16)eventScrollTimes[n], 0xf);
    screen_line(x, FONT_CHARH * i, eventScroll[n], 0xf);
    screen_int(CHAR_COLS - 5, FONT_CHARH * i, eventScrollData[n], 0xf);
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

/*
// display hmatrix SFR contents
static void show_hmatrix_sfr(void) {
  u32* reg;

  reg = AVR32_HMATRIX_SFR;
  print_dbg("\r\n HMATRIX_SFR:   ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR0;
  print_dbg("\r\n HMATRIX_SFR0:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR1;
  print_dbg("\r\n HMATRIX_SFR1:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR2;
  print_dbg("\r\n HMATRIX_SFR2:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR3;
  print_dbg("\r\n HMATRIX_SFR3:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR4;
  print_dbg("\r\n HMATRIX_SFR4:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR5;
  print_dbg("\r\n HMATRIX_SFR5:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR6;
  print_dbg("\r\n HMATRIX_SFR6:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR7;
  print_dbg("\r\n HMATRIX_SFR7:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR8;
  print_dbg("\r\n HMATRIX_SFR8:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR9;
  print_dbg("\r\n HMATRIX_SFR9:  ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR10;
  print_dbg("\r\n HMATRIX_SFR10: ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR11;
  print_dbg("\r\n HMATRIX_SFR11: ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR12;
  print_dbg("\r\n HMATRIX_SFR12: ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR13;
  print_dbg("\r\n HMATRIX_SFR13: ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR14;
  print_dbg("\r\n HMATRIX_SFR14: ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  reg = AVR32_HMATRIX_SFR15;
  print_dbg("\r\n HMATRIX_SFR15: ");
  print_dbg("address:");
  print_dbg_hex(reg);
  print_dbg(" ; value:");
  print_dbg_hex(*reg);

  print_dbg("\r\n\n");
}
*/

// display SDRAM timing constants
static void show_sdram_constants(void) {
  print_dbg("\r\n\n SDRAM constants: \r\n");
  print_dbg("\n part header: ");
  print_dbg(SDRAM_PART_HDR);
  print_dbg("\r\n");
//! The number of bank bits for this SDRAM (1 or 2).
  print_dbg("\n bank bits: "); print_dbg_ulong(  SDRAM_BANK_BITS                 );

//! The number of row bits for this SDRAM (11 to 13).
  print_dbg("\r\n row bits:"); print_dbg_ulong(  SDRAM_ROW_BITS                  );

//! The number of column bits for this SDRAM (8 to 11).
  print_dbg("\r\n col bits:"); print_dbg_ulong(  SDRAM_COL_BITS                 );

//! The minimal column address select (READ) latency for this SDRAM (1 to 3 SDRAM cycles).
//! Unit: tCK (SDRAM cycle period).
  print_dbg("\r\n CAS latency: "); print_dbg_ulong(  SDRAM_CAS                       );

//! The minimal write recovery time for this SDRAM (0 to 15 SDRAM cycles).
//! Unit: ns.
  print_dbg("\r\n T_WR: "); print_dbg_ulong(  SDRAM_TWR                       );

//! The minimal row cycle time for this SDRAM (0 to 15 SDRAM cycles).
//! ACTIVE-to-ACTIVE command delay.
//! Unit: ns.
  print_dbg("\r\n T_RC: "); print_dbg_ulong(  SDRAM_TRC                       );

//! The minimal row precharge time for this SDRAM (0 to 15 SDRAM cycles).
//! PRECHARGE command period.
//! Unit: ns.
  print_dbg("\r\n T_RP: "); print_dbg_ulong(  SDRAM_TRP                       );

//! The minimal row to column delay time for this SDRAM (0 to 15 SDRAM cycles).
//! ACTIVE-to-READ/WRITE command delay.
//! Unit: ns.
  print_dbg("\r\n T_RCD: "); print_dbg_ulong(  SDRAM_TRCD                      );

//! The minimal row address select time for this SDRAM (0 to 15 SDRAM cycles).
//! ACTIVE-to-PRECHARGE command delay.
//! Unit: ns.
  print_dbg("\r\n T_RAS: "); print_dbg_ulong(  SDRAM_TRAS                      );

//! The minimal exit self refresh time for this SDRAM (0 to 15 SDRAM cycles).
//! Exit SELF REFRESH to ACTIVE command delay.
//! Unit: ns.
  print_dbg("\r\n T_XSR: "); print_dbg_ulong(  SDRAM_TXSR                      );

//! The maximal refresh time for this SDRAM (0 to 4095 SDRAM cycles).
//! Refresh period.
//! Unit: ns.
  print_dbg("\r\n T_R: "); print_dbg_ulong(  SDRAM_TR                        );

//! The minimal refresh cycle time for this SDRAM.
//! AUTO REFRESH command period.
//! Unit: ns.
  print_dbg("\r\n T_RFC: "); print_dbg_ulong(  SDRAM_TRFC                      );

//! The minimal mode register delay time for this SDRAM.
//! LOAD MODE REGISTER command to ACTIVE or REFRESH command delay.
//! Unit: tCK (SDRAM cycle period).
  print_dbg("\r\n T_MRD: "); print_dbg_ulong(  SDRAM_TMRD                      );

//! The minimal stable-clock initialization delay for this SDRAM.
//! Unit: us.
  print_dbg("\r\n STABLE_CLOCK_INIT_DELAY: "); print_dbg_ulong(  SDRAM_STABLE_CLOCK_INIT_DELAY   );

//! The minimal number of AUTO REFRESH commands required during initialization for this SDRAM.
  print_dbg("\r\n INIT_AUTO_REFRESH_COUNT: "); print_dbg_ulong(  SDRAM_INIT_AUTO_REFRESH_COUNT  );
  print_dbg("\r\n\n");

}
