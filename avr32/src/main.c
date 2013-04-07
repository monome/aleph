/* main.c
 * avr32
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
#include "gpio.h"
//#include "intc.h"
#include "eic.h"
#include "interrupt.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"
//#include "sdramc.h"
#include "spi.h"
#include "sysclk.h"
#include "uhc.h"
#include "usart.h"
//// aleph
// bees
#include "menu.h"
#include "net.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"
// common
#include "files.h"
#include "param_common.h"
#include "screen.h"
#include "simple_string.h"
#include "types.h"
#include "util.h"
// avr32
#include "adc.h"
#include "app_timers.h"
#include "bfin.h"
#include "conf_aleph.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "fix.h"
#include "font.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "switches.h"
#include "timers.h"


//// aleph
// bees
#include "menu.h"
#include "net.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"
// common
#include "files.h"
#include "param_common.h"
#include "screen.h"
#include "simple_string.h"
#include "types.h"
#include "util.h"
// avr32
#include "adc.h"
#include "app_timers.h"
#include "bfin.h"
#include "conf_aleph.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "fix.h"
#include "font.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "switches.h"

//==================================================
//====  variables

// control network, statically allocated
ctlnet_t ctlnet;
//  flag to wait for startup button press
static u8 startup = 1;
// mode switch
static u8 mode = 0;

//=================================================
//==== declarations

static void init_avr32(void);
static void init_ctl(void);
static void check_events(void);

//=================================================
//==== definitons

// top-level peripheral init
static void init_avr32(void) {
  volatile avr32_tc_t *tc = APP_TC;
  // clocks
  init_clocks();
  // interrupt vectors
  irq_initialize_vectors();
  // disable all interrupts for now
  cpu_irq_disable();
  // serial usb
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
  // enable interrupts
  cpu_irq_enable();

  // usb host controller
  init_usb_host();

  print_dbg("\r\n avr32 init done ");
}

// control / network / logic init
static void init_ctl(void) {
  // disable interrupts
  
  cpu_irq_disable();
  // intialize the event queue
  init_events();
  // intialize encoders
  init_encoders();
  // intialize switches (debouncing)
  init_switches();
  
#if FIXMEM
  //memory manager
#else
  init_mem();
#endif
  
  // set up file navigation
  

  /////// WTF ????????????????????????????????????????????????????????????????????
  init_files();
  
  // send ADC config
  init_adc();
  // start application timers
  init_app_timers();

  preset_init();
  scene_init();
  menu_init();
  // enable interrupts

  
  //// BEES:
  net_init(&ctlnet);
  preset_init();
  scene_init();
  menu_init();
  // enable interrupts
  
  cpu_irq_enable();
}

// app event loop
static void check_events(void) {
  static event_t e;  
  static u64 delay;
  if( get_next_event(&e) ) {
    if(startup) {
      if( e.eventType == kEventSwitchDown0
	  || e.eventType == kEventSwitchDown1
	  || e.eventType == kEventSwitchDown2
	  || e.eventType == kEventSwitchDown3
	  || e.eventType == kEventSwitchDown4
	  ) {  
	startup = 0;
	return;
      }
    } else {
    
      switch(e.eventType) {

      case kEventRefresh:
	screen_refresh();
	break;
      case kEventSwitchDown0:
	menu_handleKey(eKeyFnDownA, e.eventData);
	break;
      case kEventSwitchUp0:
	menu_handleKey(eKeyFnUpA, e.eventData);
	break;
      case kEventSwitchDown1:
	//      print_dbg("\r\n switch f1 down");
	menu_handleKey(eKeyFnDownB, e.eventData);
	break;
      case kEventSwitchUp1:
	//	print_dbg("\r\n switch f1 up");
	menu_handleKey(eKeyFnUpB, e.eventData);
	break;
      case kEventSwitchDown2:
	//	print_dbg("\r\n switch f2 down");
	menu_handleKey(eKeyFnDownC, e.eventData);
	break;
      case kEventSwitchUp2:
	//	print_dbg("\r\n switch f2 up");
	menu_handleKey(eKeyFnUpC, e.eventData);
	break;
      case kEventSwitchDown3:
	menu_handleKey(eKeyFnDownD, e.eventData);
	break;
      case kEventSwitchUp3:
	menu_handleKey(eKeyFnUpD, e.eventData);
	break;
      case kEventSwitchDown4:
	mode ^= 1;
	if(mode) { gpio_set_gpio_pin(LED_MODE_PIN); }
	else { gpio_clr_gpio_pin(LED_MODE_PIN); }
	menu_handleKey(eKeyMode, e.eventData);
	break;
      case kEventSwitchUp4:
	break;
      case kEventSwitchDown5:
	screen_line(0, 0, "powering down!", 0x3f);
	print_dbg("\r\n AVR32 received power down switch event");
	screen_refresh();
	delay = 50000; while(--delay) {;;}
	gpio_clr_gpio_pin(POWER_CTL_PIN);
	break;
      case kEventSwitchUp5:
	break;

      case kEventEncoder0:
	//	print_dbg("\r\n encoder 0");
      	if(e.eventData > 0) {
      	  menu_handleKey(eKeyEncUpD, e.eventData);
      	} else {
      	  menu_handleKey(eKeyEncDownD, e.eventData);
      	}
      	break;
      case kEventEncoder1:
	//	print_dbg("\r\n encoder 1");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpC, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownC, e.eventData);
	}
	break;
      case kEventEncoder2:
	//	print_dbg("\r\n encoder 2");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpB, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownB, e.eventData);
	}
	break;
      case kEventEncoder3:
	//	print_dbg("\r\n encoder 3");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpA, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownA, e.eventData);
	}
	break;

      case kEventAdc0:
	//	print_dbg("\r\nadc val 0: ");
	//	print_dbg_hex(e.eventData);
	//	displayAdcVal(0, e.eventData);
	break;
      case kEventAdc1:
	//	 print_dbg("\r\nadc val 1: ");
	//	 print_dbg_hex(e.eventData);
	 //	 displayAdcVal(1, e.eventData);
	break;
      case kEventAdc2:
	//	 print_dbg("\r\nadc val 2: ");
	//	 print_dbg_hex(e.eventData);
	//	 displayAdcVal(2, e.eventData);
	break;
      case kEventAdc3:
	//     	print_dbg("\r\nadc val 3: ");
	//     	print_dbg_hex(e.eventData);
	//     	displayAdcVal(3, e.eventData);
	break;
      }
    } // if event
  } // if !startup
}


////main function
int main (void) {
  u32 waitForCard = 0;
  volatile u64 delay;

  // set up avr32 hardware and peripherals
  init_avr32();  

  // wait for sd card
  screen_line(0, 0, "ALEPH", 0x3f);
  screen_line(0, 1, "waiting for SD card...", 0x3f);
  screen_refresh();
  
  print_dbg("\r\n SD check... ");
  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
  }
  print_dbg("\r\nfound SD card. ");

  screen_blank_line(0, 0);
  screen_blank_line(0, 1);
  screen_line(0, 0, "SD card detected.", 0x3f);
  screen_line(0, 1, "press any key to continue...", 0x3f);
  screen_refresh();

  // setup control logic
  init_ctl();


  /* /// boot default dsp */
  /* files_load_dsp_name("default.ldr"); */
  /* /// wait for bfin to finish boot */
  /* delay = 500000; while(delay--) {;;} */
  /* /// again... */
  /* delay = 500000; while(delay--) {;;} */
  
  /* // populate control network with poarameters as reported by bfin */
  /* report_params(); */

  while(1) {
    check_events(); 
  }
}
