/*
  handler.c
  bees

  app-specific UI event handlers.
  
  app.c defines a global array of function pointers to handle system events.
  main.c defines its own handlers, most of which don't do anything.
  some events (e.g. ftdiConnect, monomePoll) call pretty low-level driver stuff,
  and there shouldn't be any need for bees or other applications to customize them.

  handlers that do need to be customized go here.
  define static functions and then populate the global array with pointers to these functions.
  this step should happen in e.g. app_launch().

*/

// asf
#include "gpio.h"
#include "delay.h"
#include "print_funcs.h"

// aleph-avr32
#include "aleph_board.h"
#include "app.h"
#include "event_types.h"

// bees
#include "app_timers.h"
//#include "files.h"
#include "handler.h"
#include "render.h"

///-------------------------------------
///---- static event handlers


static void handle_Adc0(s32 data) { 
  //... no ADC
}

static void handle_Adc1(s32 data) { 
  //... no ADC
}

static void handle_Adc2(s32 data) { 
  //... no ADC
}

static void handle_Adc3(s32 data) { 
  //... no ADC
}

static void handle_Switch0(s32 data) { 
  ctl_toggle_mute(0);
}

static void handle_Switch1(s32 data) { 
  ctl_toggle_mute(1);
}

static void handle_Switch2(s32 data) { 
  ctl_toggle_mute(2);
}

static void handle_Switch3(s32 data) { 
  ctl_toggle_mute(3);
}

static void handle_Switch4(s32 data) { 
  // mode switch
  //... 
}

static void handle_Switch5(s32 data) { 
  /// power switch
  // we are powering down, save current settings
  //...
  delay_ms(100);
  gpio_clr_gpio_pin(POWER_CTL_PIN);
}

static void handle_Switch6(s32 data) {
  // footswitch 1
}

static void handle_Switch7(s32 data) { 
  // footswitch 2
} 

static void handle_MonomeConnect(s32 data) { 
  // if we were using a grid...
  // timers_set_monome();
}

static void handle_MonomeDisconnect(s32 data) { 
  // if we were using a grid...
  //  timers_unset_monome();
}

static void handle_MonomeGridKey(s32 data) { 
  //...
}

static void handle_MonomeGridTilt(s32 data) { 
  //...
}

static void handle_MonomeRingEnc(s32 data) {
  //...
}

static void handle_MonomeRingKey(s32 data) { 
  //...
}

static void handle_MidiConnect(s32 data) {
  //  timers_set_midi();
}

static void handle_MidiDisconnect(s32 data) { 
  //  timers_unset_midi();
}

static void handle_MidiPacket(s32 data) {
  //...
}

static void handle_HidConnect(s32 data) {
  //...
}

static void handle_HidDisconnect(s32 data) {
  //...
}

static void handle_HidPacket(s32 data) {
  //...
}

static void handle_Serial(s32 data) {
  //  serial_process(data);
}

//-------------------------------------
//---- extern

/// explicitly assign these...
/// this way the order of the event types enum doesn't matter.
void assign_event_handlers(void) {
  /// if you need to spawn events in the app to be deferred to main loop..
  //  app_event_handlers[ kEventAppCustom ]  = ...

  // system-defined:
  app_event_handlers[ kEventAdc0 ]	= &handle_Adc0 ;
  app_event_handlers[ kEventAdc1 ]	= &handle_Adc1 ;
  app_event_handlers[ kEventAdc2 ]	= &handle_Adc2 ;
  app_event_handlers[ kEventAdc3 ]	= &handle_Adc3 ;
  // power/mode/footswitches here, fn switches in page handlers
  app_event_handlers[ kEventSwitch4 ]	= &handle_Switch4 ;
  app_event_handlers[ kEventSwitch5 ]	= &handle_Switch5 ;
  app_event_handlers[ kEventSwitch6 ]	= &handle_Switch6 ;
  app_event_handlers[ kEventSwitch7 ]	= &handle_Switch7 ;
  app_event_handlers[ kEventMonomeConnect ]	= &handle_MonomeConnect ;
  app_event_handlers[ kEventMonomeDisconnect ]	= &handle_MonomeDisconnect ;
  app_event_handlers[ kEventMonomeGridKey ]	= &handle_MonomeGridKey ;
  app_event_handlers[ kEventMonomeGridTilt ]	= &handle_MonomeGridTilt ;
  app_event_handlers[ kEventMonomeRingEnc ]	= &handle_MonomeRingEnc ;
  app_event_handlers[ kEventMonomeRingKey ]	= &handle_MonomeRingKey ;
  app_event_handlers[ kEventMidiConnect ]	= &handle_MidiConnect ;
  app_event_handlers[ kEventMidiDisconnect ]	= &handle_MidiDisconnect ;
  app_event_handlers[ kEventMidiPacket ]	= &handle_MidiPacket ;
  app_event_handlers[ kEventHidConnect ]	= &handle_HidConnect ;
  app_event_handlers[ kEventHidDisconnect ]	= &handle_HidDisconnect ;
  app_event_handlers[ kEventHidPacket ]	= &handle_HidPacket ;

  app_event_handlers[ kEventSerial ] = &handle_Serial ;
}
