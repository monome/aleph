/* main.c
 * 
 * avr32 interface board test
 */

#include <avr32/io.h>

//// test;
#include "board.h"
//////

#include "compiler.h"
#include "gpio.h"
#include "intc.h"

#include "config.h"
#include "init.h"
#include "screen.h"
//#include "font.h"
#include "events.h"
#include "eventTypes.h"
#include "timers.h"
#include "encoders.h"


//-----------------------------
//---- static variables

// switch pin array
static const U16 swPins[4] = {
  SW0_PIN,
  SW1_PIN,
  SW2_PIN,
  SW3_PIN,
};

/////////////
//// TEST
static S32 encVal[4] = { 0, 0, 0, 0 };
////////////

//--------------------
//--- static functions

// interrupt handler for PB00-PB07
static void int_handler_port1_line0(void);
// register interrupts
static void register_interrupts(void);
// generate events from switch interrupts
//static void handle_sw( const U8 swIdx );
// UI event loop
static void check_events(void);


// interrupt handler for PA00-PA07
__attribute__((__interrupt__))
static void int_handler_port0_line0(void) {
 U8 i; 

  // check for encoder movement:
  for(i = 0; i<NUM_ENC; i++) {
    if(gpio_get_pin_interrupt_flag(enc[i].pin[0])) {
      process_enc(i);
      gpio_clear_pin_interrupt_flag(enc[i].pin[0]);
    }
    if(gpio_get_pin_interrupt_flag(enc[i].pin[1])) {
      process_enc(i);
      gpio_clear_pin_interrupt_flag(enc[i].pin[1]);
    }
  }
}


// interrupt handler for PB00-PB07
__attribute__((__interrupt__))
static void int_handler_port1_line0(void) {
  U8 i;

  /////// TEST:
  gpio_tgl_gpio_pin(LED1_GPIO);
  /////////
 
}


// register interrupts
static void register_interrupts(void) {
  // enable interrupts on encoder pins
  gpio_enable_pin_interrupt( ENC0_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC0_S1_PIN,	GPIO_PIN_CHANGE);
  
  /*
  gpio_enable_pin_interrupt( ENC1_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC1_S1_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC2_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC2_S1_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC3_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC3_S1_PIN,	GPIO_PIN_CHANGE);
  */

  // register PB00-PB07 and assign interrupt handler
  //INTC_register_interrupt( &int_handler_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), AVR32_INTC_INT1 );

  // testing ENC0 on PA06, PA07...   
 INTC_register_interrupt( &int_handler_port0_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA00 / 8), AVR32_INTC_INT1 );
    
}

//=========================================== 
// application event loop
static void check_events(void) {
  static event_t e;

  ///// debug
  // static S8 encValLast = 0;
  // static U8 i = 0;
  ///// 
  
  U8 refresh = 0;

  if( get_next_event(&e) ) {
    switch(e.eventNum) {

    case kEventEncoder0:
      /*
      ///// debug
      if(e.eventData == encValLast) {
	i++;
      }
      if((e.eventData != 1) && (e.eventData != -1)) {
	i++;
      }
      encValLast = e.eventData;
      ///////
      */
      encVal[0] += e.eventData;
      screen_draw_int(0, SCREEN_LINE(0), encVal[0], 0x0f);
      refresh = 1;
      break;

    default:
      break;
    }
  }

  if(refresh) {
    screen_refresh();
  }

}

//===========================================
// main function
int main(void) {  

   // initialize AVR32 peripherals
  init_avr();

  // disable interrupts
  Disable_global_interrupt();

  // initialize the OLED screen
  init_oled();
  // intialize the event queue
  init_events();
  // intialize encoders
  init_encoders();
  // register interrupts
  register_interrupts();

  // enable interrupts
  Enable_global_interrupt();

  // check for application events in an infinite loop
  while(1) {
    check_events();
  }
  
  return 0;
}

