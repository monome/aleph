/* main.c
 * 
 * avr32 interface board test
 */

// asf
#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
#include "intc.h"
#include "tc.h"
#include "delay.h"
/// debug
#include "sysclk.h"

// aleph
#include "config.h"
#include "init.h"
#include "screen.h"
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

// tick counter for main app timer
static U64 tcTicks;
// display refresh flag
static U8 refresh = 0;

//--------------------
//--- static functions
// interrupt handler for PA00-PA07
static void irq_port0_line0(void);
// interrupt handler for PB00-PB07
//static void irq_port1_line0(void);
// interrupt handler for TC
static void irq_tc(void);
// register interrupts
static void register_interrupts(void);
// generate events from switch interrupts
//static void handle_sw( const U8 swIdx );
// UI event loop
static void check_events(void);

// interrupt handler for PA00-PA07
__attribute__((__interrupt__))
static void irq_port0_line0(void) {
  U8 i; 
  //  gpio_tgl_gpio_pin(LED1_GPIO);
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


/*
// interrupt handler for PB00-PB07
__attribute__((__interrupt__))
static void irq_port1_line0(void) {
  U8 i;
  /////// TEST:
  gpio_tgl_gpio_pin(LED1_GPIO);
  ///////// 
}
*/

// timer irq
__attribute__((__interrupt__))
static void irq_tc(void) {
  event_t e;
  tcTicks++;

  // Clear the interrupt flag. This is a side effect of reading the TC SR.
  tc_read_sr(APP_TC, APP_TC_CHANNEL);
  if(refresh) {
    e.eventType = kEventRefresh;
    post_event(&e);
    refresh = 0;
  }
}

// register interrupts
static void register_interrupts(void) {
  // Initialize interrupt vectors.
  INTC_init_interrupts();

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
  
 INTC_register_interrupt( &irq_port0_line0,
			  AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA24 / 8),
			  AVR32_INTC_INT2 );

 // register TC interrupt
 INTC_register_interrupt(&irq_tc,
			 APP_TC_IRQ,
			 APP_TC_IRQ_PRIORITY);
 
}
 

//=========================================== 
// application event loop
static void check_events(void) {
  static event_t e;
  
  if( get_next_event(&e) ) {
    switch(e.eventType) {

    case kEventEncoder0:
      encVal[0] += e.eventData;
      screen_draw_int(0, SCREEN_LINE(0), encVal[0], 0x0f);
      refresh = 1;
      //    screen_refresh();
      break;
      
    case kEventRefresh:
      screen_refresh();
      refresh = 0;
      break;
     
    default:
      break;
    }

    //////// TEST
    // if(refresh) { screen_refresh(); refresh = 0; }
    /////////
  }
}

//===========================================
// main function
int main(void) {  
  //  int dum;
  volatile avr32_tc_t *tc = APP_TC;

  // clocks
  init_clocks();
  // disable interrupts
  cpu_irq_disable();

  // GPIO
  init_gpio();
  // USARTs
  init_usart();

  // initialize the OLED screen
  init_oled();

  //  delay_ms(20);

  // register interrupts
  register_interrupts();

  // intialize the event queue
  init_events();
  // intialize encoders
  init_encoders();
  // timer/counter
  init_tc(tc);

  //  delay_ms(20);

  // enable interrupts
  cpu_irq_enable();

  screen_draw_string_squeeze(0, FONT_CHARH * 2, "cpu clock speed:", 0xf);
  screen_draw_int(0, FONT_CHARH * 3, sysclk_get_cpu_hz(), 0x0f);
  screen_refresh();

  // check for application events in an infinite loop
  
  while(1) {
    check_events();
  }
  
  return 0;
}
