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
// common
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
//#include "app_timers.h"
#include "bfin.h"
#include "conf_tc_irq.h"
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
#include "monome.h"
#include "switches.h"
#include "sys_timers.h"
#include "timers.h"

//==================================================
//====  defines

// run an event handler with NULL check
// #define APP_HANDLE_EVENT(handler, args) if( handler != NULL) (*handler)(args)

//==================================================
//====  extern variables

/* // handlers for function switches */
/* sw_handler fnSwHandler[4] = { NULL, NULL, NULL, NULL }; */
/* // handler for mode switch */
/* sw_handler modeSwHandler = NULL; */
/* // handlers for footswitches */
/* sw_handler footSwHandler[2] = { NULL, NULL }; */
/* // handlers for the 4 encoders */
/* enc_handler encHandler[4] = { NULL, NULL, NULL, NULL }; */
/* // handlers for adcs */
/* adc_handler adcHandler[4] = { NULL, NULL, NULL, NULL }; */
/* // monome grid key handler */
/* monome_key_handler gridHandler = NULL; */

event_handler appEventHandler;

//==================================================
//====  static variables
// flag for firstrun
static u8 firstrun = 0;
//  flag to wait for startup button press
static u8 startup = 1;

//=================================================
//==== static declarations

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

  // not sure why but when need to explictly enable clock for static mem ctlr
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
  // initialize usb classes
  init_monome();
  //  init_midi();
  //  init_hid();

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

  // start timers
  init_sys_timers();
  //  init_app_timers();
  print_dbg("\r\n init_timers");

  // enable interrupts
  cpu_irq_enable();

}

// app event loop
static void check_events(void) {
  /* static event_t e; */
  /* u8 launch = 0; */
  /* //  print_dbg("\r\n checking events..."); */
  /* if( get_next_event(&e) ) { */
  /* /\* print_dbg("\r\n handling event, type: "); *\/ */
  /* /\* print_dbg_hex(e.eventType); *\/ */
  /* /\* print_dbg("\r\n , data: "); *\/ */
  /* /\* print_dbg_hex(e.eventData); *\/ */

  /*   if(startup) { */
  /*     if( e.eventType == kEventSwitch0 */
  /* 	  || e.eventType == kEventSwitch1 */
  /* 	  || e.eventType == kEventSwitch2 */
  /* 	  || e.eventType == kEventSwitch3 */
  /* 	  || e.eventType == kEventSwitch4 */
  /* 	  ) { */
  /* 	startup = 0; */
  /* 	print_dbg("\r\n key pressed, launching "); */
  /* 	// return 1 if app completed firstrun tasks */
  /* 	launch = app_launch(firstrun); */
  /* 	delay_ms(10); */
  /* 	if( firstrun) { */
  /* 	  //	  if(launch) { */
  /* 	    // successfully launched on firstrun, so write magic number to flash */
  /* 	    flash_write_firstrun(); */
  /* 	    //	    return; */
  /* 	    //	  } else { */
  /* 	    // firstrun, but app launch failed, so clear magic number to try again */
  /* 	    flash_clear_firstrun(); */
  /* 	    //	  }  */
  /* 	} */
  /*     } */
  /*   } else { */
  /*     switch(e.eventType) { */
	
  /*     case kEventRefresh: */
  /* 	// refresh the screen hardware */
  /* 	//	screen_refresh(); */
  /* 	break; */
  /*     case kEventMonomePoll : */
  /* 	// poll monome serial input and spawn relevant events */
  /* 	monome_read_serial(); */
  /* 	break; */
  /*     case kEventMonomeRefresh : */
  /* 	// refresh monome device from led state buffer */
  /* 	monome_grid_refresh(); */
  /* 	break; */
  /* 	//-------------------------------------- */
  /*     case kEventFtdiConnect: */
  /* 	// perform setup tasks for new ftdi device connection.  */
  /* 	// won't work if called from an interrupt. */
  /* 	ftdi_setup(); */
  /* 	break; */
  /*     default: */
  /* 	(*appEventHandler)(&e); */
  /* 	break; */
  /*     } // event switch */
  /*   } // startup */
  /* } // got event */
}

//int main(void) {
////main function
int main (void) {
  //  u32 waitForCard = 0;

  // set up avr32 hardware and peripherals
  init_avr32();


  print_dbg("\r\n SRAM size: 0x");
  print_dbg_hex(smc_get_cs_size(1));


  cpu_irq_disable();
  /// test the SRAM
  sram_test();

  cpu_irq_enable();

  /* //memory manager */
  /* init_mem();   */
  /* print_dbg("\r\n init_mem"); */

  /* // intialize the FAT filesystem */
  /* fat_init(); */
  /* print_dbg("\r\n init fat"); */

  /* // setup control logic */
  /* init_ctl(); */
  /* print_dbg("\r\n init ctl"); */

  /* // initialize the application */
  /* app_init(); */
  /* print_dbg("\r\n init app"); */

  /* // initialize flash */
  /* firstrun = init_flash(); */
  /* print_dbg("r\n init flash, firstrun: "); */
  /* print_dbg_ulong(firstrun); */

  /* // notify  */
  /* screen_startup(); */

  /* print_dbg("\r\n starting event loop.\r\n"); */

  /* while(1) { */
  /*   check_events(); */
  /* } */
}