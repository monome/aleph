// ASF
#include "compiler.h"
#include "delay.h"
#include "gpio.h"
#include "intc.h"
#include "print_funcs.h"
#include "tc.h"
// aleph
#include "aleph_board.h"
#include "conf_tc_irq.h"
#include "global.h"
#include "interrupts.h"
#include "types.h"

//#define UI_IRQ_PRIORITY AVR32_INTC_INT2

/// toggle output on GPIO
u8 tog = 0;

//------------------------
//----- variables
// timer tick counter
volatile u64 tcTicks = 0;
volatile u8 tcOverflow = 0;
static const u64 tcMax = (U64)0x7fffffff;
static const u64 tcMaxInv = (u64)0x10000000;
// screen refresh flag
volatile u8 refresh = 0;
// end of PDCA transfer
//volatile bool end_of_transfer;

//----------------------
//---- static functions 
// interrupt handlers

// irq for app timer
__attribute__((__interrupt__))
static void irq_tc(void);

// irq for PA24-PA31
__attribute__((__interrupt__))
static void irq_port0_line3(void);

// irq for PB00-PB07
__attribute__((__interrupt__))
static void irq_port1_line0(void);

// irq for PB08-PB15
__attribute__((__interrupt__))
static void irq_port1_line1(void);

// irq for PB16-PB23
/* __attribute__((__interrupt__)) */
/* static void irq_port1_line2(void); */

// irq for PB24-PB31
__attribute__((__interrupt__))
static void irq_port1_line3(void);

//---------------------------------
//----- static function definitions

__attribute__((__interrupt__))
static void irq_tc(void) {
  // clear interrupt flag by reading timer SR
  tc_read_sr(APP_TC, APP_TC_CHANNEL);
}

// interrupt handler for PA23-PA30
__attribute__((__interrupt__))
static void irq_port0_line3(void) {
  //  print_dbg("\r\n interrupt on port0_line3");
  //SW_F0
  if(gpio_get_pin_interrupt_flag(SW0_PIN)) {
    gpio_clear_pin_interrupt_flag(SW0_PIN);
  }
  // SW_F1
  if(gpio_get_pin_interrupt_flag(SW1_PIN)) {
    gpio_clear_pin_interrupt_flag(SW1_PIN);
  }
  // SW_F2
  if(gpio_get_pin_interrupt_flag(SW2_PIN)) {
    gpio_clear_pin_interrupt_flag(SW2_PIN);
  }
 
  // SW_F3
  if(gpio_get_pin_interrupt_flag(SW3_PIN)) {
    gpio_clear_pin_interrupt_flag(SW3_PIN);
  }
  // SW_MODE
  if(gpio_get_pin_interrupt_flag(SW_MODE_PIN)) {
    gpio_clear_pin_interrupt_flag(SW_MODE_PIN);
  }
}

// interrupt handler for PB00-PB07
__attribute__((__interrupt__))
static void irq_port1_line0(void) {
  // print_dbg("\r\b\interrupt on PB00-PB07.");
  // ENC0_0
  if(gpio_get_pin_interrupt_flag(ENC0_S0_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC0_S0_PIN);
  }  
  // ENC0_1
  if(gpio_get_pin_interrupt_flag(ENC0_S1_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC0_S1_PIN);
  }
  // ENC1_0
  if(gpio_get_pin_interrupt_flag(ENC1_S0_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC1_S0_PIN);
  }  
  // ENC1_1
  if(gpio_get_pin_interrupt_flag(ENC1_S1_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC1_S1_PIN);
  }
  // ENC2_0
  if(gpio_get_pin_interrupt_flag(ENC2_S0_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC2_S0_PIN);
  }  
  // ENC2_1
  if(gpio_get_pin_interrupt_flag(ENC2_S1_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC2_S1_PIN);
  }
}

// interrupt handler for PB08-PB15
__attribute__((__interrupt__))
static void irq_port1_line1(void) {
  //    print_dbg("\r\b\interrupt on PB08-PB15.");
  // ENC3_0
  if(gpio_get_pin_interrupt_flag(ENC3_S0_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC3_S0_PIN);
  }  
  // ENC3_1
  if(gpio_get_pin_interrupt_flag(ENC3_S1_PIN)) {
    gpio_clear_pin_interrupt_flag(ENC3_S1_PIN);
  }
}

// interrupt handler for PB24-PB31
__attribute__((__interrupt__))
static void irq_port1_line3(void) {
  //  print_dbg("\r\n irq_port1_line3");
  if(gpio_get_pin_interrupt_flag(FS0_PIN)) {
    gpio_clear_pin_interrupt_flag(FS0_PIN);
  }
  if(gpio_get_pin_interrupt_flag(FS1_PIN)) {
    gpio_clear_pin_interrupt_flag(FS1_PIN);
  }
}

  //-----------------------------
  //---- external function definitions

  // register interrupts
  void register_interrupts(void) {
    // enable interrupts on GPIO inputs

    // BFIN_HWAIT
    // gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_RISING_EDGE);

    // encoders
    gpio_enable_pin_interrupt( ENC0_S0_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( ENC0_S1_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( ENC1_S0_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( ENC1_S1_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( ENC2_S0_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( ENC2_S1_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( ENC3_S0_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( ENC3_S1_PIN,	GPIO_PIN_CHANGE);

    // switches
    gpio_enable_pin_interrupt( SW0_PIN,	        GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( SW1_PIN,	        GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( SW2_PIN,	        GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( SW3_PIN,	        GPIO_PIN_CHANGE);

    gpio_enable_pin_interrupt( FS0_PIN,	GPIO_PIN_CHANGE);
    gpio_enable_pin_interrupt( FS1_PIN,	GPIO_PIN_CHANGE);

    gpio_enable_pin_interrupt( SW_MODE_PIN,	GPIO_PIN_CHANGE);
    //  gpio_enable_pin_interrupt( SW_POWER_PIN,	GPIO_PIN_CHANGE);
 
    // PA24 - PA31
    INTC_register_interrupt( &irq_port0_line3, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA24 / 8), UI_IRQ_PRIORITY);

    // PB00 - PB07
    INTC_register_interrupt( &irq_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), UI_IRQ_PRIORITY );

    // PB08 - PB15
    INTC_register_interrupt( &irq_port1_line1, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB08 / 8), UI_IRQ_PRIORITY);

    // PB16 - PB23
    //  INTC_register_interrupt( &irq_port1_line2, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB16 / 8), UI_IRQ_PRIORITY);

    // PB24 - PB31
    INTC_register_interrupt( &irq_port1_line3, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB24 / 8), UI_IRQ_PRIORITY);

    // register TC interrupt
    INTC_register_interrupt(&irq_tc, APP_TC_IRQ, APP_TC_IRQ_PRIORITY);
  }
