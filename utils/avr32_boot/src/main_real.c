/* main.c
 * avr32
 * aleph
 *
 */

// ASF
#include "compiler.h"
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "ctrl_access.h"
#include "delay.h"
#include "flashc.h"
#include "intc.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "pm.h"
#include "gpio.h"
#include "sd_mmc_spi.h"
#include "smc.h"
#include "sysclk.h"

//// aleph
// bees
#include "menu.h"
#include "net.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"
// common
#include "files.h"
#include "fix.h"
#include "param_common.h"
#include "screen.h"
#include "simple_string.h"
#include "types.h"
#include "util.h"
// avr32
#include "aleph_board.h"
#include "adc.h"
#include "app_timers.h"
#include "bfin.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "flash.h"
#include "font.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "switches.h"
#include "timers.h"


//==================================================
//====  variables

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
  // setup clocks
  sysclk_init();
  // why need here?
  sysclk_enable_pbb_module(SYSCLK_SMC_REGS);
  flashc_set_bus_freq(FCPU_HZ);
  flashc_set_wait_state(1);

  /// interrupts
  irq_initialize_vectors();

  // disable all interrupts for now
  cpu_irq_disable();
  // serial usb
  init_ftdi_usart();
  // external sram
  smc_init(FHSB_HZ);
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
  print_dbg("\r\n init_events");

  // intialize encoders
  init_encoders();
  print_dbg("\r\n init_encoders");

  //memory manager
  init_mem();  
  print_dbg("\r\n init_mem");

  // send ADC config
  init_adc();
  print_dbg("\r\n init_adc");

  // start application timers
  init_app_timers();
  print_dbg("\r\n init_timers");
  
  //// BEES:
  net_init();
  print_dbg("\r\n net_init");

  preset_init();
  print_dbg("\r\n preset_init");

  scene_init();
  print_dbg("\r\n scene_init");

  menu_init();
  print_dbg("\r\n menu_init");

  // enable interrupts
  cpu_irq_enable();
}

// app event loop
static void check_events(void) {
  static event_t e;  

  if( get_next_event(&e) ) {

  /* print_dbg("\r\n handling event, type: "); */
  /* print_dbg_hex(e.eventType); */
  /* print_dbg("\r\n , data: "); */
  /* print_dbg_hex(e.eventData); */

    if(startup) {
      if( e.eventType == kEventSwitchDown0
	  || e.eventType == kEventSwitchDown1
	  || e.eventType == kEventSwitchDown2
	  || e.eventType == kEventSwitchDown3
	  || e.eventType == kEventSwitchDown4
	  ) {  
	startup = 0;
	/// FIXME: should go to some default UI state here
	return;
      }
    } else {
    
      switch(e.eventType) {

      case kEventRefresh:
	screen_refresh();
	break;
	//----- function switches
      case kEventSwitchDown0: 
	menu_handleKey(eKeyFnDownA, e.eventData);
	break;
      case kEventSwitchUp0:
	menu_handleKey(eKeyFnUpA, e.eventData);
	break;
      case kEventSwitchDown1:
	menu_handleKey(eKeyFnDownB, e.eventData);
	break;
      case kEventSwitchUp1:
	menu_handleKey(eKeyFnUpB, e.eventData);
	break;
      case kEventSwitchDown2:
	menu_handleKey(eKeyFnDownC, e.eventData);
	break;
      case kEventSwitchUp2:
	menu_handleKey(eKeyFnUpC, e.eventData);
	break;
      case kEventSwitchDown3:
	menu_handleKey(eKeyFnDownD, e.eventData);
	break;
      case kEventSwitchUp3:
	menu_handleKey(eKeyFnUpD, e.eventData);
	break;
	/// footswitches
      case kEventSwitchDown6:
	print_dbg("\r\n footswitch1 down");
	break;
      case kEventSwitchUp6:
	print_dbg("\r\n footswitch1 up");
	break;
      case kEventSwitchDown7:
	print_dbg("\r\n footswitch2 down");
	break;
      case kEventSwitchUp7:
	print_dbg("\r\n footswitch2 up");
	break;
	// mode switch
      case kEventSwitchDown4:
	mode ^= 1;
	if(mode) { gpio_set_gpio_pin(LED_MODE_PIN); }
	else { gpio_clr_gpio_pin(LED_MODE_PIN); }
	menu_handleKey(eKeyMode, e.eventData);
	break;
      case kEventSwitchUp4:
	break;
	// power switch
      case kEventSwitchDown5:
	screen_line(0, 0, "powering down!", 0x3f);
	print_dbg("\r\n AVR32 received power down switch event");
	screen_refresh();
	gpio_clr_gpio_pin(POWER_CTL_PIN);
	break;
      case kEventSwitchUp5:
	break;
      case kEventEncoder0:
			print_dbg("\r\n encoder 0");
      	if(e.eventData > 0) {
      	  menu_handleKey(eKeyEncUpD, e.eventData);
      	} else {
      	  menu_handleKey(eKeyEncDownD, e.eventData);
      	}
      	break;
      case kEventEncoder1:
	print_dbg("\r\n encoder 1");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpC, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownC, e.eventData);
	}
	break;
      case kEventEncoder2:
	print_dbg("\r\n encoder 2");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpB, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownB, e.eventData);
	}
	break;
      case kEventEncoder3:
	print_dbg("\r\n encoder 3");
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

//int main(void) {
////main function
int main (void) {
  u32 waitForCard = 0;


  ////////////////
  /////////////
  /// TEST: flash
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
  init_dbg_rs232(FOSC0);
  test_flash();
  return 1;
  ////////
  //////////////
  ///////////




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

  /// initialize filesystem
  init_files();

  // setup control logic
  init_ctl();

  /// boot default dsp
  screen_line(0, 1, "loading default DSP...", 0x3f);
  screen_refresh();
  files_load_dsp_name("default.ldr");

  screen_line(0, 1, "finished. press any key to continue...", 0x3f);
  screen_refresh();

  print_dbg("\r\n starting event loop.\r\n");


  //////
  //  test_flash();
  ////

  while(1) {
    check_events();
  }
}
