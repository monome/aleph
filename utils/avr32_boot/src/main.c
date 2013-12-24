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
// #include "print_funcs.h"
#include "pm.h"
#include "gpio.h"
#include "wdt.h"
#include "sd_mmc_spi.h"
#include "smc.h"
#include "sysclk.h"
#include "wdt.h"

//// aleph
// bees
#include "menu.h"
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
#include "app_timers.h"
#include "bfin.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "flash.h"
#include "font.h"
#include "global.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "switches.h"
#include "timers.h"
#include "watchdog.h"

//==================================================
//====  variables

//  flag to wait for startup button press
// static u8 startup = 1;
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
  // external sram
  smc_init(FHSB_HZ);
  // initialize spi1: OLED, SD/MMC
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

  // // print_dbg("\r\n avr32 init done ");
}

// control / network / logic init
static void init_ctl(void) {
  // disable interrupts
    cpu_irq_disable();

  // intialize the event queue
  init_events();
  // // print_dbg("\r\n init_events");

  // intialize encoders
  init_encoders();
  // // print_dbg("\r\n init_encoders");

  //memory manager
  init_mem();  
  // // print_dbg("\r\n init_mem");
  // start application timers
  init_app_timers();

  // // print_dbg("\r\n init_timers");
  menu_init();

  // // print_dbg("\r\n menu_init");

  // enable interrupts
  cpu_irq_enable();
}

// app event loop
static void check_events(void) {
  static event_t e;  

  if( get_next_event(&e) ) {

    
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
 //      case kEventSwitchDown1:
	// menu_handleKey(eKeyFnDownB, e.eventData);
	// break;
 //      case kEventSwitchUp1:
	// menu_handleKey(eKeyFnUpB, e.eventData);
	// break;
 //      case kEventSwitchUp2:
	// menu_handleKey(eKeyFnUpC, e.eventData);
	// break;

	// // mode switch
 //      case kEventSwitchDown4:
	// // mode ^= 1;
	// // if(mode) { gpio_set_gpio_pin(LED_MODE_PIN); }
	// // else { gpio_clr_gpio_pin(LED_MODE_PIN); }
	// menu_handleKey(eKeyMode, e.eventData);
	// break;
	// // power switch
 //      case kEventSwitchDown5:
	// //	screen_line(0, 0, "powering down!", 0x3f);
	// //	// print_dbg("\r\n AVR32 received power down switch event");
	// // screen_refresh();
	// gpio_clr_gpio_pin(POWER_CTL_PIN);
	// break;
 //      case kEventEncoder0:
	// 		// // print_dbg("\r\n encoder 0");
 //      	if(e.eventData > 0) {
 //      	  menu_handleKey(eKeyEncUpD, e.eventData);
 //      	} else {
 //      	  menu_handleKey(eKeyEncDownD, e.eventData);
 //      	}
 //      	break;
 //      case kEventEncoder1:
	// // // print_dbg("\r\n encoder 1");
	// if(e.eventData > 0) {
	//   menu_handleKey(eKeyEncUpC, e.eventData);
	// } else {
	//   menu_handleKey(eKeyEncDownC, e.eventData);
	// }
	// break;
      case kEventEncoder2:
	// // print_dbg("\r\n encoder 2");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpB, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownB, e.eventData);
	}
	break;
      case kEventEncoder3:
	// // print_dbg("\r\n encoder 3");
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpA, e.eventData);
	} else {
	  menu_handleKey(eKeyEncDownA, e.eventData);
	}
	break;


    } // if event
  } // if !startup
}

//int main(void) {
////main function
int main (void) {
  wdt_disable();

  u8 isFirstRun = 0;
  u8 isSwDown = 0;
  
  /// trying this...
  //  wdt_clear();
  //  wdt_disable();

  /// check pin and jump out
  gpio_enable_pin_pull_up(SW_MODE_PIN);

  isSwDown = gpio_get_pin_value(SW_MODE_PIN);

  if(!isSwDown) {
    /// hardcoded jump to runtime code location
    asm volatile (
    		  " mov   r0,LO(0x80008000)\n\t"
    		  " orh   r0,HI(0x80008000)\n\t"
    		  " mov   pc,r0"
    		  );
  } else {
    // set up avr32 hardware and peripherals
    init_avr32();

    // wait for sd card
    // screen_line(0, 0, "ALEPH BOOTLOADER", 0x3f);
    // screen_refresh();
  
    //    // print_dbg("\r\n SD check... ");
    if (!sd_mmc_spi_mem_check()) {
      screen_line(0, 1, "no SD card found.", 0x3f);
      screen_line(0, 2, "resetting...", 0x3f);
      screen_refresh();
      delay_ms(1000);
      watchdog_reset();
      
    }
    // initialize higher-level program stuff
    init_ctl();

    // initialize system file functions
    init_files();

    // flash init, check firstrun
    isFirstRun = init_flash();

    delay_ms(500);
    menu_refresh();

  }

  while(1) {
    check_events();
  }
}


// testing flash

/* static u32 flashoff =0x8000ff00; */
/* static void print_flash(void) { */
/*   u32 i, j; */
/*   u32 b, boff; */
/*   // print_dbg("\r\n"); */
/*   // print_dbg_hex(flashoff); */
/*   // print_dbg(" : "); */
/*   for(j=0; j<8; j++) { */
/*     b = 0; */
/*     boff = 24; */
/*     for(i=0; i<4; i++) { */
/*       b |= ( *(u8*)flashoff ) << boff; */
/*       flashoff++; */
/*       boff -= 8; */
/*     } */
/*     // print_dbg_hex(b); */
/*     // print_dbg(" "); */
/*   } */

/*     //  // print_dbg_hex(flash); */
/*   //  flashOff += 4; */
/* } */

