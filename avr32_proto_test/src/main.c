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
//  app event loop
static void check_events(void);
// wait for first UI event after startup
static U8 check_init(void);
// get params from bfinx
static void report_params(void);
// check for debug event commands
static char check_debug_events(void);

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
        
    case kEventSwitchDown0:
      //      if (check_init()) { return; }
      //      key = eKeyFnDownA;
      print_dbg("\r\n switch down");
      gpio_set_gpio_pin(AVR32_PIN_PB00);
      break;
    case kEventSwitchUp0:
      print_dbg("\r\n switch up");
      gpio_clr_gpio_pin(AVR32_PIN_PB00);
      //      key = eKeyFnUpA;
      break;
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
  // send ADC config
  init_adc();
  init_app_timers();

  print_dbg("starting event loop.\n\r");

  // event loop
    while(1) {
      check_debug_events();
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
