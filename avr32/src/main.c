/* main,c
 * aleph-avr32
 *
 */


//ASF
#include <string.h>
//#include <sysclk.h>
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "compiler.h"
#include "ctrl_access.h"
#include "delay.h"
#include "gpio.h"
#include "intc.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "sysclk.h"
// aleph
#include "bfin.h"
#include "conf_aleph.h"
#include "encoders.h"
#include "events.h"
#include "files.h"
#include "global.h"
#include "init.h"
#include "interrupts.h"
#include "screen.h"

//=========================================
//==== static variables
S32 encVal[4] = { 0, 0, 0, 0 };

//=========================================== 
//==== static functions
static void check_events(void);
static void post_num_params(void);

// application event loop
static void check_events(void) {
  static event_t e;
  
  if( get_next_event(&e) ) {
    switch(e.eventType) {

    case kEventEncoder0:
      encVal[0] += e.eventData;
      screen_draw_int(0, SCREEN_LINE(0), encVal[0], 0x0f);
      print_dbg("\r\nencoder 0 value: ");
      print_dbg_ulong(encVal[0]);
      if(encVal[0] > 0) {
	bfin_set_param(1, encVal[0]);
      }
      refresh = 1;
      break;
      
    case kEventSwitchDown0:
      post_num_params();
      break;

    case kEventRefresh:
      screen_refresh();
      refresh = 0;
      break;
     
    default:
      break;
    }
  }
}

////main function
int main (void) {
  U32 waitForCard = 0;
  volatile avr32_tc_t *tc = APP_TC;
  
  // switch to osc0 for main clock
  //  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 
  // initialize clocks:
  init_clocks();
  
  // initialize Interrupt Controller
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
  print_dbg("\r\nALEPH ");
  print_dbg("\r\nwaiting for SD card... ");
  // Wait for a card to be inserted
  
  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
  }
  print_dbg("\r\ncard detected! ");

  // set up file navigation using available drives
  init_files();

  // list files
  files_list();
  
  // load blackfin from first .ldr file in filesystem
  load_bfin_sdcard_test();

  print_dbg("done.\n\r");

  //// wait!
  delay_ms(30);
 
  //bfin_hack(7);
  //post_num_params();

  print_dbg("starting event loop.\n\r");
  // event loop
  while(1) {
    check_events();
  }
}

static void report_params(void) {
  U8 col;
  U8 numParams;
  print_dbg("\n\requesting parameters..."); 
  numParams =  bfin_get_num_params();
  print_dbg("done.\n\r");

  col = screen_draw_string_squeeze(0, FONT_CHARH * 2, "found parameters:", 0xf);
  col++;
  screen_draw_int(col, FONT_CHARH * 2, numParams, 0x0f);
  screen_refresh();

}
