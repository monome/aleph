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
#include "app.h"
#include "app_timers.h"
#include "bfin.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "flash.h"
#include "font.h"
#include "ftdi.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "switches.h"
#include "timers.h"


//==================================================
//====  variables
// flag for firstrun
static u8 firstrun = 0;
//  flag to wait for startup button press
static u8 startup = 1;
// mode switch
// static u8 mode = 0;

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

  // send ADC config
  init_adc();
  print_dbg("\r\n init_adc");

  // start application timers
  init_app_timers();
  print_dbg("\r\n init_timers");

  // enable interrupts
  cpu_irq_enable();

  // initialize the application
  app_init();

}

// app event loop
static void check_events(void) {
  static event_t e;
  //  print_dbg("\r\n checking events...");
  if( get_next_event(&e) ) {
  /* print_dbg("\r\n handling event, type: "); */
  /* print_dbg_hex(e.eventType); */
  /* print_dbg("\r\n , data: "); */
  /* print_dbg_hex(e.eventData); */

    if(e.eventType == kEventFtdiConnect) {
	// perform setup tasks for new ftdi device connection. 
	// won't work if called from an interrupt.
	ftdi_setup();
    }


    if(startup) {
      if( e.eventType == kEventSwitchDown0
	  || e.eventType == kEventSwitchDown1
	  || e.eventType == kEventSwitchDown2
	  || e.eventType == kEventSwitchDown3
	  || e.eventType == kEventSwitchDown4
	  ) {
	startup = 0;
	app_launch(firstrun);
	return;
      }
    } else {
      switch(e.eventType) {
      case kEventFtdiRead :
	// check the FTDI buffer and spawn related events.
	// this won't work if called from the timer interrupt.
	ftdi_read();
	break;
      /* case kEventFtdiWrite : */
      /* 	ftdi_write((u32)e.eventData); */
      /* 	break; */
      case kEventSwitchDown5 :
	screen_line(0, 0, "powering down!", 0x3f);
	print_dbg("\r\n AVR32 received power down switch event");
	screen_refresh();
	gpio_clr_gpio_pin(POWER_CTL_PIN);
	break;
	//// test: switches -> monome
      case kEventSwitchDown0:
	ftdi_write(1);
	break;
      case kEventSwitchUp0:
	ftdi_write(0);
	break;
      case kEventSwitchDown2:
	ftdi_write(0x2003);
	break;
      case kEventSwitchDown3:
	ftdi_write(0x2004);
	break;
	////
      default:
	// all other events are sent to application layer
	app_handle_event(&e);
	break;
      } // event switch
    } // startup
  } // got event
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

  //memory manager
  init_mem();  
  print_dbg("\r\n init_mem");

  /// initialize filesystem
  ////// FIXME: move to app
  files_init();

  // setup control logic
  init_ctl();

  // initialize flash
  firstrun = init_flash();

  // notify 
  screen_clear();
  screen_line(0, 1, "press any key to continue...", 0x3f);
  screen_refresh();

  print_dbg("\r\n starting event loop.\r\n");


  while(1) {
    check_events();
  }
}


