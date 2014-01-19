/* main.c
   aleph-avr32
   
   calls hardware initialization
   processes event queue,
   sets default event handlers
 
 */

// asf
/* #include "compiler.h" */
/* #include "board.h" */
/* #include "conf_sd_mmc_spi.h" */
/* #include "ctrl_access.h" */
/* #include "delay.h" */
/* #include "flashc.h" */
/* #include "gpio.h" */
/* #include "intc.h" */
/* #include "pdca.h" */
/* #include "power_clocks_lib.h" */
 #include "print_funcs.h" 
/* #include "pm.h" */
/* #include "sd_mmc_spi.h" */
/* #include "smc.h" */
/* #include "sysclk.h" */
/* #include "usart.h" */

//// aleph
// common
#include "fix.h"
#include "param_common.h"
#include "screen.h"
#include "simple_string.h"
#include "types.h"
#include "util.h"
// avr32
//#include "aleph_board.h"
#include "adc.h"
#include "app.h"
#include "bfin.h"
//#include "conf_tc_irq.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "flash.h"
#include "font.h"
//#include "ftdi.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "monome.h"
#include "serial.h"
#include "switches.h"
#include "timers.h"

/////////////
//// do nothing
#if 1

int main(void) { 

  app_init();
  app_launch(0);

  return 0; 
}

#else
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

//==================================================
//====  defines

//==================================================
//====  extern variables

//==================================================
//====  static variables
// flag for firstrun
static u8 firstrun = 0;
//  flag to wait for startup button press
static u8 launch = 0;
// flags for device connection events.
// need to re-send after app launch.
static u8 ftdiConnect = 0;
static u8 monomeConnect = 0;
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

// core event handlers
static void handler_Adc0(s32 data) { ;; }
static void handler_Adc1(s32 data) { ;; }
static void handler_Adc2(s32 data) { ;; }
static void handler_Adc3(s32 data) { ;; }
static void handler_Encoder0(s32 data) { ;; } 
static void handler_Encoder1(s32 data) { ;; } 
static void handler_Encoder2(s32 data) { ;; } 
static void handler_Encoder3(s32 data) { ;; } 
static void handler_Switch0(s32 data) { check_startup(); }
static void handler_Switch1(s32 data) { check_startup(); }
static void handler_Switch2(s32 data) { check_startup(); }
static void handler_Switch3(s32 data) { check_startup(); }
static void handler_Switch4(s32 data) { ;; }
static void handler_Switch5(s32 data) { ;; }
static void handler_Switch6(s32 data) { ;; }
static void handler_Switch7(s32 data) { ;; }
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
    monomeConnect = 1;
  }
}

static void handler_MonomeDisconnect(s32 data) { ;; }
static void handler_MonomePoll(s32 data) {
  monome_read_serial();
}
static void handler_MonomeRefresh(s32 data) {
  (*monome_refresh)();
  // FIXME: arc?
}

static void handler_MonomeGridKey(s32 data) { ;; }
static void handler_MonomeGridTilt(s32 data) { ;; }
static void handler_MonomeRingEnc(s32 data) { ;; }
static void handler_MonomeRingKey(s32 data) { ;; }
static void handler_MidiConnect(s32 data) { 
  if(!launch) {
    midiConnect = 1;
  }
}
static void handler_MidiDisconnect(s32 data) { ;; }
static void handler_MidiPacket(s32 data) { ;; }
static void handler_MidiRefresh(s32 data) {
  // TODO
}
static void handler_HidConnect(s32 data) { 
  if(!launch) {
    hidConnect = 1;
  }
}
static void handler_HidDisconnect(s32 data) { ;; }
static void handler_HidByte(s32 data) { ;; }

static void handler_SerialParamNum(s32 data) { serial_param_num(data); }
static void handler_SerialParamInfo(s32 data) { serial_param_info(data); }
static void handler_SerialParamGet(s32 data) { serial_param_get(data); }
static void handler_SerialParamSet(s32 data) { serial_param_set(data); }

static void handler_AppCustom(s32 data) { ;; }

/// explicitly assign default event handlers.
/// this way the order of the event types enum doesn't matter.
static inline void assign_main_event_handlers(void) {
  app_event_handlers[ kEventAdc0]	= &handler_Adc0 ;
  app_event_handlers[ kEventAdc1 ]	= &handler_Adc1 ;
  app_event_handlers[ kEventAdc2 ]	= &handler_Adc2 ;
  app_event_handlers[ kEventAdc3 ]	= &handler_Adc3 ;
  app_event_handlers[ kEventEncoder0 ]	= &handler_Encoder0 ;
  app_event_handlers[ kEventEncoder1 ]	= &handler_Encoder1 ;
  app_event_handlers[ kEventEncoder2 ]	= &handler_Encoder2 ;
  app_event_handlers[ kEventEncoder3 ]	= &handler_Encoder3 ;
  app_event_handlers[ kEventSwitch0 ]	= &handler_Switch0 ;
  app_event_handlers[ kEventSwitch1 ]	= &handler_Switch1 ;
  app_event_handlers[ kEventSwitch2 ]	= &handler_Switch2 ;
  app_event_handlers[ kEventSwitch3 ]	= &handler_Switch3 ;
  app_event_handlers[ kEventSwitch4 ]	= &handler_Switch4 ;
  app_event_handlers[ kEventSwitch5 ]	= &handler_Switch5 ;
  app_event_handlers[ kEventSwitch6 ]	= &handler_Switch6 ;
  app_event_handlers[ kEventSwitch7 ]	= &handler_Switch7 ;
  app_event_handlers[ kEventFtdiConnect ]	= &handler_FtdiConnect ;
  app_event_handlers[ kEventFtdiDisconnect ]	= &handler_FtdiDisconnect ;
  app_event_handlers[ kEventMonomeConnect ]	= &handler_MonomeConnect ;
  app_event_handlers[ kEventMonomeDisconnect ]	= &handler_MonomeDisconnect ;
  app_event_handlers[ kEventMonomePoll ]	= &handler_MonomePoll ;
  app_event_handlers[ kEventMonomeRefresh ]	= &handler_MonomeRefresh ;
  app_event_handlers[ kEventMonomeGridKey ]	= &handler_MonomeGridKey ;
  app_event_handlers[ kEventMonomeGridTilt ]	= &handler_MonomeGridTilt ;
  app_event_handlers[ kEventMonomeRingEnc ]	= &handler_MonomeRingEnc ;
  app_event_handlers[ kEventMonomeRingKey ]	= &handler_MonomeRingKey ;
  app_event_handlers[ kEventMidiConnect ]	= &handler_MidiConnect ;
  app_event_handlers[ kEventMidiDisconnect ]	= &handler_MidiDisconnect ;
  app_event_handlers[ kEventMidiPacket ]	= &handler_MidiPacket ;
  app_event_handlers[ kEventMidiRefresh ]	= &handler_MidiRefresh ;
  app_event_handlers[ kEventHidConnect ]	= &handler_HidConnect ;
  app_event_handlers[ kEventHidDisconnect ]	= &handler_HidDisconnect ;
  app_event_handlers[ kEventHidByte ]	= &handler_HidByte ;

  app_event_handlers[ kEventSerialParamNum ] = &handler_SerialParamNum ;
  app_event_handlers[ kEventSerialParamInfo ] = &handler_SerialParamInfo ;
  app_event_handlers[ kEventSerialParamGet ] = &handler_SerialParamGet ;
  app_event_handlers[ kEventSerialParamSet ] = &handler_SerialParamSet ;

  app_event_handlers[ kEventAppCustom ]	= &handler_AppCustom ;
}

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
    launch = app_launch(firstrun);

    delay_ms(10);

    if(launch) {
      if(firstrun) {
	// successfully launched on firstrun, so write magic number to flash
	flash_write_firstrun();
      }
      // re-send connection events if we got any
      if(ftdiConnect) {
	e.type = kEventFtdiConnect;
	event_post(&e);
      } 
      if(monomeConnect) {
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
  print_dbg("r\n init flash, firstrun: ");
  print_dbg_ulong(firstrun);

  // check sw2 and force firstrun if held
  if(gpio_get_pin_value(SW2_PIN)) {
    firstrun = 1;
    print_dbg("r\n sw2 down -> force firstrun ");
  }

  // assign default event handlers
  assign_main_event_handlers();

  print_dbg("\r\n starting event loop.\r\n");

  check_startup();

  while(1) {
    check_events();
  }
}


#endif
