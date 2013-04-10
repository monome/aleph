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
//#include "conf_aleph.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "fix.h"
#include "font.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
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
  //init_clocks();
  // interrupt vectors

  // disable all interrupts for now
  cpu_irq_disable();
  // serial usb
  //  init_ftdi_usart();

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

  /// initialize filesystem
   init_files();

  // usb host controller
   //   init_usb_host();

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
  // intialize switches (debouncing)
  init_switches();
  print_dbg("\r\n init_switches");
  
/* #if FIXMEM */
/*   //memory manager */
/* #else */
/*   //  init_mem(); */
/* #endif */
  
  // set up file navigation
  
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
	///	      print_dbg("\r\n switch f1 down");
	menu_handleKey(eKeyFnDownB, e.eventData);
	break;
      case kEventSwitchUp1:
	//		print_dbg("\r\n switch f1 up");
	menu_handleKey(eKeyFnUpB, e.eventData);
	break;
      case kEventSwitchDown2:
	//		print_dbg("\r\n switch f2 down");
	menu_handleKey(eKeyFnDownC, e.eventData);
	break;
      case kEventSwitchUp2:
	//		print_dbg("\r\n switch f2 up");
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
	gpio_clr_gpio_pin(POWER_CTL_PIN);
	break;
      case kEventSwitchUp5:
	break;

      case kEventEncoder0:
	//		print_dbg("\r\n encoder 0");
      	if(e.eventData > 0) {
      	  menu_handleKey(eKeyEncUpD, e.eventData);
      	} else {
      	  menu_handleKey(eKeyEncDownD, e.eventData);
      	}
      	break;
      case kEventEncoder1:
	//		print_dbg("\r\n encoder 1");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpC, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownC, e.eventData);
	}
	break;
      case kEventEncoder2:
	//		print_dbg("\r\n encoder 2");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpB, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownB, e.eventData);
	}
	break;
      case kEventEncoder3:
	//		print_dbg("\r\n encoder 3");
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

// startup routine runs before main()
int _init_startup(void);
int _init_startup(void) {

  // Import the Exception Vector Base Address.
  extern void _evba;

  // setup clocks
  flashc_set_wait_state( 1 );
  sysclk_init();
  sysclk_enable_pbb_module(SYSCLK_SMC_REGS);

  // Switch to external oscillator 0.
  //pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
  
  // Load the Exception Vector Base Address in the corresponding system register
  Set_system_register(AVR32_EVBA, (int)&_evba);

  // Enable exceptions.
  Enable_global_exception();

  // Initialize interrupt handling.
  INTC_init_interrupts();
  init_dbg_rs232(FPBA_HZ);

  // setup static memory controller 
  smc_init(FHSB_HZ);
    //  smc_init(FOSC0);
  // return dont-care
  return 1;
}

//int main(void) {
////main function
int main (void) {
  u32 waitForCard = 0;

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

  /// boot default dsp
  files_load_dsp_name("default.ldr");


  // keep power alive (cleared by int)
  gpio_set_gpio_pin(POWER_CTL_PIN);
  
  print_dbg("\r\n starting event loop.\r\n");
  while(1) {
    check_events();
  }
}
