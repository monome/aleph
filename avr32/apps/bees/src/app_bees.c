/*
  app_bees.c
  aleph-avr32
  bees

  required app-specific implementation of avr32/src/app.h
 */

// asf
#include "gpio.h"
#include "print_funcs.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "flash.h"
#include "screen.h"
// bees
#include "files.h"
#include "menu.h"
#include "monome.h"
#include "play.h"
#include "scene.h"


static u8 keyMode = 0;


//// test monome grid handling
static void test_handle_monome_grid(event_t* ev) {
  u8 x, y, z;
  monome_grid_read_event(ev, &x, &y, &z);
  //bees_grid_operator_activate(.... );
  monome_grid_led(x, y, z);
}


void app_handle_event(event_t* e) {
  //// APP 
  switch(e->eventType) {

  case kEventRefresh:
    screen_refresh();
    break;
    //----- function switches
  case kEventSwitchDown0:
    menu_handleKey(eKeyFnDownA, e->eventData);
    break;
  case kEventSwitchUp0:
    menu_handleKey(eKeyFnUpA, e->eventData);
    break;
  case kEventSwitchDown1:
    menu_handleKey(eKeyFnDownB, e->eventData);
    break;
  case kEventSwitchUp1:
    menu_handleKey(eKeyFnUpB, e->eventData);
    break;
  case kEventSwitchDown2:
    menu_handleKey(eKeyFnDownC, e->eventData);
    break;
  case kEventSwitchUp2:
    menu_handleKey(eKeyFnUpC, e->eventData);
    break;
  case kEventSwitchDown3:
    menu_handleKey(eKeyFnDownD, e->eventData);
    break;
  case kEventSwitchUp3:
    menu_handleKey(eKeyFnUpD, e->eventData);
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
    keyMode ^= 1;
    if(keyMode) { gpio_set_gpio_pin(LED_MODE_PIN); }
    else { gpio_clr_gpio_pin(LED_MODE_PIN); }
    menu_handleKey(eKeyMode, e->eventData);
    break;
  case kEventSwitchUp4:
    break;

  case kEventSwitchUp5:
    break;
  case kEventEncoder0:
    //	 print_dbg("\r\n encoder 0");
    if(e->eventData > 0) {
      menu_handleKey(eKeyEncUpD, e->eventData);
    } else {
      menu_handleKey(eKeyEncDownD, e->eventData);
    }
    break;
  case kEventEncoder1:
    //	print_dbg("\r\n encoder 1");
    if(e->eventData > 0) {
      menu_handleKey(eKeyEncUpC, e->eventData);
    } else {
      menu_handleKey(eKeyEncDownC, e->eventData);
    }
    break;
  case kEventEncoder2:
    //	print_dbg("\r\n encoder 2");
    if(e->eventData > 0) {
      menu_handleKey(eKeyEncUpB, e->eventData);
    } else {
      menu_handleKey(eKeyEncDownB, e->eventData);
    }
    break;
  case kEventEncoder3:
    //	print_dbg("\r\n encoder 3");
    if(e->eventData > 0) {
      menu_handleKey(eKeyEncUpA, e->eventData);
    } else {
      menu_handleKey(eKeyEncDownA, e->eventData);
    }
    break;

  case kEventMonomeGrid:
    //   print_dbg("\r\n app_bees handling monome grid event");
    test_handle_monome_grid(e);
    break;
  case kEventMonomeArc:
    break;
  case kEventMonomeTilt:
    break;



  case kEventAdc0:
    //    print_dbg("\r\nadc val 0: ");
    //    print_dbg_hex(e->eventData);
    //    displayAdcVal(0, e->eventData);
    break;
  case kEventAdc1:
    //    print_dbg("\r\nadc val 1: ");
    //    print_dbg_hex(e->eventData);
    //    displayAdcVal(1, e->eventData);
    break;
  case kEventAdc2:
    //    print_dbg("\r\nadc val 2: ");
    //    print_dbg_hex(e->eventData);
    //    displayAdcVal(2, e->eventData);
    break;
  case kEventAdc3:
    //    print_dbg("\r\nadc val 3: ");
    //    print_dbg_hex(e->eventData);
    //    displayAdcVal(3, e->eventData);
    break;
  }
}

// this is called during hardware initialization.
// use for memory allocation.
void app_init(void) {
  net_init();
  print_dbg("\r\n net_init");
  
  preset_init();
  print_dbg("\r\n preset_init");

  scene_init();
  print_dbg("\r\n scene_init");

  /// FIXME: this is arbitrarily but sensitively located in main.c right now...
  //  files_init();
  //  print_dbg("\r\n files_init");

  menu_init();
  print_dbg("\r\n menu_init");

  play_init();
  print_dbg("\r\n play_init");
}

// this is called from the event queue 
void app_launch(u8 firstrun) {
  app_notify("launching BEES");
  print_dbg("\r\n launching app with firstrun: ");
  print_dbg_ulong(firstrun);
  if(firstrun) {
    scene_write_default();
  } else {
    print_dbg("\r\n booting default ldr from flash... ");

    flash_read_ldr();
    bfin_load_buf();

    scene_read_default();
  }
  menu_refresh();
}
