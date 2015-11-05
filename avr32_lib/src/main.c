/* main.c
   aleph-avr32
   
   calls hardware initialization
   processes event queue,
   sets default event handlers
 
 */

// asf
#include "compiler.h"
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "ctrl_access.h"
#include "delay.h"
#include "flashc.h"
#include "gpio.h"
#include "intc.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "pm.h"
#include "sd_mmc_spi.h"
#include "smc.h"
#include "sysclk.h"
#include "usart.h"

//// aleph
// common
#include "fix.h"
#include "param_common.h"
#include "screen.h"
#include "simple_string.h"
#include "types.h"
//#include "util.h"
// avr32
#include "aleph_board.h"
#include "adc.h"
#include "app.h"
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
#include "serial.h"
#include "switches.h"
#include "timers.h"

//==================================================
//====  defines

//==================================================
//====  extern variables

//==================================================
//====  static variables
// flag for firstrun
static u8 firstrun = 0;
// flag for clean start
static u8 cleanstart = 0;

//  flag to wait for startup button press
static u8 launch = 0;
// flags for device connection events.
// need to re-send after app launch.
static u8 ftdiConnect = 0;
static u8 monomeConnectMain = 0;
static u8 hidConnect = 0;
static u8 midiConnect = 0;

//=================================================
//==== static declarations

// initializations
static void init_avr32(void);
static void init_ctl(void);
// check the event queue
static void check_events(void);
// check startup status and possibly launch the application
static void check_startup(void);

// dummies
static void dummy_handler(s32 data) { ;; }
//static void dummy_render(void) { ;; }

// core event handlers
static void handler_FtdiConnect(s32 data) {
  ftdi_setup();
}
static void handler_FtdiDisconnect(s32 data) { 
  /// FIXME: assuming that FTDI == monome
  event_t e = { .type = kEventMonomeDisconnect };
  event_post(&e);
}

static void handler_MonomeConnect(s32 data) {
  // this just stores a flag to re-send connection event to app
  if(!launch) {
    // print_dbg("\r\n got monome device connection, saving flag for app launch");
    monomeConnectMain = 1;
  }
}

static void handler_MonomePoll(s32 data) {
  monome_read_serial();
}

static void handler_MonomeRefresh(s32 data) {
  (*monome_refresh)();
  // FIXME: arc?
}

static void handler_MidiConnect(s32 data) { 
  if(!launch) {
    midiConnect = 1;
  }
}

static void handler_MidiRefresh(s32 data) {
  // TODO ??
}

static void handler_HidConnect(s32 data) { 
  if(!launch) {
    hidConnect = 1;
  }
}

// to be app defined
static void handler_Serial(s32 data) { 
    ;;
}

/// explicitly assign default event handlers.
/// this way the order of the event types enum doesn't matter.
static inline void assign_main_event_handlers(void) {
  app_event_handlers[ kEventAdc0]	= &dummy_handler ;
  app_event_handlers[ kEventAdc1 ]	= &dummy_handler ;
  app_event_handlers[ kEventAdc2 ]	= &dummy_handler ;
  app_event_handlers[ kEventAdc3 ]	= &dummy_handler ;
  app_event_handlers[ kEventEncoder0 ]	= &dummy_handler ;
  app_event_handlers[ kEventEncoder1 ]	= &dummy_handler ;
  app_event_handlers[ kEventEncoder2 ]	= &dummy_handler ;
  app_event_handlers[ kEventEncoder3 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch0 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch1 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch2 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch3 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch4 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch5 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch6 ]	= &dummy_handler ;
  app_event_handlers[ kEventSwitch7 ]	= &dummy_handler ;
  app_event_handlers[ kEventFtdiConnect ]	= &handler_FtdiConnect ;
  app_event_handlers[ kEventFtdiDisconnect ]	= &handler_FtdiDisconnect ;
  app_event_handlers[ kEventMonomeConnect ]	= &handler_MonomeConnect ;
  app_event_handlers[ kEventMonomeDisconnect ]	= &dummy_handler ;
  app_event_handlers[ kEventMonomePoll ]	= &handler_MonomePoll ;
  app_event_handlers[ kEventMonomeRefresh ]	= &handler_MonomeRefresh ;
  app_event_handlers[ kEventMonomeGridKey ]	= &dummy_handler ;
  app_event_handlers[ kEventMonomeGridTilt ]	= &dummy_handler ;
  app_event_handlers[ kEventMonomeRingEnc ]	= &dummy_handler ;
  app_event_handlers[ kEventMonomeRingKey ]	= &dummy_handler ;
  app_event_handlers[ kEventMidiConnect ]	= &handler_MidiConnect ;
  app_event_handlers[ kEventMidiDisconnect ]	= &dummy_handler ;
  app_event_handlers[ kEventMidiPacket ]	= &dummy_handler ;
  app_event_handlers[ kEventMidiRefresh ]	= &handler_MidiRefresh ;
  app_event_handlers[ kEventHidConnect ]	= &handler_HidConnect ;
  app_event_handlers[ kEventHidDisconnect ]	= &dummy_handler ;
  app_event_handlers[ kEventHidPacket ]	= &dummy_handler ;

  app_event_handlers[ kEventSerial ] = &handler_Serial;
}

//=================================================
//==== definitons

// low-level init called by startup_uc3.S assembly.  So we can init
// the static mem ctlr before C runtime starts up and use external SDRAM for the heap.
int _init_startup(void);
int _init_startup(void) {
  // clocks
  // setup clocks
  sysclk_init();

  // not sure why but when need to explictly enable clock for static mem ctlr
  sysclk_enable_pbb_module(SYSCLK_SMC_REGS);
  flashc_set_bus_freq(FCPU_HZ);
  flashc_set_wait_state(1);

  /// interrupts
  irq_initialize_vectors();

  // serial usb
  init_ftdi_usart();

  print_dbg(" ++++++++++++++++ Initializing SMC / SDRAM\r\n ");

  // external sram
  smc_init(FHSB_HZ);

  // init will continue below after app main() is called from the C runtime.
  return 0;
}


// top-level peripheral init
static void init_avr32(void) {
  
 // fixme: test malloc for SDRAM paranoia
  test_malloc();

  // disable all interrupts for now
  cpu_irq_disable();

  // initialize spi1: OLED, ADC, SD/MMC
  init_spi1();
  // initialize PDCA controller
  init_local_pdca();
  // initialize blackfin resources
  init_bfin_resources();
  // initialize application timer
  init_tc();
  // initialize other GPIO
  init_gpio();
  // register interrupts
  register_interrupts();
  // initialize the OLED screen
  init_oled();
  // init twi
  init_twi();

  // enable interrupts
  cpu_irq_enable();

  // usb host controller
  init_usb_host();
  // initialize usb class drivers
  init_monome();

  print_dbg("\r\n ++++++++++++++++ avr32 init done ");
}

// init timer-related stuff.
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

  // enable interrupts
  cpu_irq_enable();
  print_dbg("\r\n enabled interrupts");
}

// launch application
void check_startup(void) {
  event_t e = { .data = 0 };

  if(!launch) {
    //// haven't launched yet
    // wipe out the event queue
    init_events();
    // clear the power sw interrupt? wtf?? ok
    gpio_clear_pin_interrupt_flag(SW_POWER_PIN);
    // return 1 if app completed firstrun tasks
    if(firstrun) { 
      launch = app_launch(eLaunchStateFirstRun);
    } else if(cleanstart) {
      launch = app_launch(eLaunchStateClean);
    } else {
      launch = app_launch(eLaunchStateNormal);
    }

    delay_ms(10);

    if(launch) {

      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // you must do this, or the aleph will powercycle forever !!
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // pull up power control pin, enabling soft-powerdown
      gpio_set_gpio_pin(POWER_CTL_PIN);

      if(firstrun) {
	// successfully launched on firstrun, so write magic number to flash
	flash_write_firstrun();
      }
      // re-send connection events if we got any
      if(ftdiConnect) {
	e.type = kEventFtdiConnect;
	event_post(&e);
      } 
      if(monomeConnectMain) {
	// print_dbg("\r\n posting MonomeConnect event after app launch");
	e.type = kEventMonomeConnect;
	event_post(&e);
      } 
      if(hidConnect) {
	e.type = kEventHidConnect;
	event_post(&e);
      } 
      if(midiConnect) {
	e.type = kEventMidiConnect;
	event_post(&e);
      } 
    } else {      
      if( firstrun) {
	// firstrun, but app launch failed, so clear magic number to try again
	flash_clear_firstrun();
      } 
    }
  }
}

// app event loop
void check_events(void) {
  static event_t e;
  if( event_next(&e) ) {
    (app_event_handlers)[e.type](e.data);
  }
}

// !!!!!!!!!!!!!
// main function
int main (void) {

  // set up avr32 hardware and peripherals
  init_avr32();

  // show the startup screen
  screen_startup();  

  //memory manager
  init_mem();  
  print_dbg("\r\n init_mem");

  // intialize the FAT filesystem
  fat_init();
  print_dbg("\r\n init filesystem");

  // setup control logic
  init_ctl();
  print_dbg("\r\n init ctl");

  // initialize the application
  app_init();
  print_dbg("\r\n init app");

  // initialize flash
  firstrun = init_flash();
  // print_dbg("r\n init flash, firstrun: ");
  // print_dbg_ulong(firstrun);

  // check sw2 and force firstrun if held
  if(gpio_get_pin_value(SW2_PIN)) {
    firstrun = 1;
    print_dbg("r\n sw2 down -> force firstrun ");
  }
  // kind of weird... why both? 
  if(gpio_get_pin_value(SW0_PIN)) {
    firstrun = 1;
    print_dbg("r\n sw0 down -> force firstrun ");
  }

  // add another pin check on SW1 for 'clean start'
  // this is to tell the application to start in a known default state,
  // without requiring flash initialization
  if(gpio_get_pin_value(SW1_PIN)) {
    cleanstart = 1;
    print_dbg("r\n sw1 down -> force clean start ");
  }


  // assign default event handlers
  assign_main_event_handlers();
  // assign default screen render
  //  app_render_screen = &(dummy_render);
  print_dbg("\r\n assigned default/dummy event handlers");
  

  print_dbg("\r\n starting event loop.\r\n");

  check_startup();

  while(1) {
    check_events();
  }
}
