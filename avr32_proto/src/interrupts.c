// ASF
#include "compiler.h"
#include "gpio.h"
#include "intc.h"
#include "pdca.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"
#include "tc.h"
// aleph
#include "bfin.h"
#include "conf_aleph.h"
#include "encoders.h"
#include "events.h"
#include "event_types.h"
#include "global.h"
#include "interrupts.h"
#include "switches.h"
#include "timers.h"


//------------------------
//----- variables
// timer tick counter
volatile U64 tcTicks = 0;
// screen refresh flag
volatile U8 refresh = 0;
// end of PDCA transfer
volatile bool end_of_transfer;

//----------------------
//---- static functions 
// interrupt handlers

// irq for pdca (sdcard)
__attribute__((__interrupt__))
static void irq_pdca(void);

// irq for app timer
__attribute__((__interrupt__))
static void irq_tc(void);

// irq for PA00-PA07
__attribute__((__interrupt__))
static void irq_port0_line0(void);

// irq for PB00-PA07
__attribute__((__interrupt__))
static void irq_port1_line0(void);

//---------------------------------
//----- static function definitions
__attribute__((__interrupt__))
static void irq_pdca(void) {
  volatile U16 delay;
  // Disable all interrupts.
  Disable_global_interrupt();

  // Disable interrupt channel.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

  sd_mmc_spi_read_close_PDCA();//unselects the SD/MMC memory.

  // wait (FIXME)
  delay=0; while(delay < 5000) { delay++; }

  // Disable unnecessary channel
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);

  // Enable all interrupts.
  Enable_global_interrupt();

  end_of_transfer = true;
}

// timer irq
__attribute__((__interrupt__))
static void irq_tc(void) {
  //event_t e;
  tcTicks++;


  process_timers();

  // 1/2 sec heartbeat LED
  
  /*
  if ((tcTicks % 500) == 0) {
    gpio_tgl_gpio_pin(AVR32_PIN_PB00);
    gpio_tgl_gpio_pin(AVR32_PIN_PB02);
    gpio_tgl_gpio_pin(AVR32_PIN_PA05);
    gpio_tgl_gpio_pin(AVR32_PIN_PA23);
    gpio_tgl_gpio_pin(AVR32_PIN_PA24);
  }
  */
  
  // clear interrupt flag by reading timer SR
  tc_read_sr(APP_TC, APP_TC_CHANNEL);

}

// interrupt handler for PA00-PA07
__attribute__((__interrupt__))
static void irq_port0_line0(void) {
  // BFIN_HWAIT: set value
  if(gpio_get_pin_interrupt_flag(BFIN_HWAIT_PIN)) {
    hwait = gpio_get_pin_value(BFIN_HWAIT_PIN);
    gpio_clear_pin_interrupt_flag(BFIN_HWAIT_PIN);
  }
}

// interrupt handler for PA23-PA30
__attribute__((__interrupt__))
static void irq_port0_line3(void) {
  // TEST
  gpio_tgl_gpio_pin(LED1_GPIO);
  // this interrupt line includes enc0 - enc2
  if(gpio_get_pin_interrupt_flag(ENC0_S0_PIN)) {
    process_enc(0);
    gpio_clear_pin_interrupt_flag(ENC0_S0_PIN);
  }  
  if(gpio_get_pin_interrupt_flag(ENC0_S1_PIN)) {
    process_enc(0);
    gpio_clear_pin_interrupt_flag(ENC0_S1_PIN);
  }
  if(gpio_get_pin_interrupt_flag(ENC1_S0_PIN)) {
    process_enc(1);
    gpio_clear_pin_interrupt_flag(ENC1_S0_PIN);
  }  
  if(gpio_get_pin_interrupt_flag(ENC1_S1_PIN)) {
    process_enc(1);
    gpio_clear_pin_interrupt_flag(ENC1_S1_PIN);
  }
  if(gpio_get_pin_interrupt_flag(ENC2_S0_PIN)) {
    process_enc(2);
    gpio_clear_pin_interrupt_flag(ENC2_S0_PIN);
  }  
  if(gpio_get_pin_interrupt_flag(ENC2_S1_PIN)) {
    process_enc(2);
    gpio_clear_pin_interrupt_flag(ENC2_S1_PIN);
  }
}

// interrupt handler for PB00-PB07
__attribute__((__interrupt__))
static void irq_port1_line0(void) {
  // this interrupt line includes enc3....
  if(gpio_get_pin_interrupt_flag(ENC3_S0_PIN)) {
    process_enc(3);
    gpio_clear_pin_interrupt_flag(ENC3_S0_PIN);
  }  
  if(gpio_get_pin_interrupt_flag(ENC3_S1_PIN)) {
    process_enc(3);
    gpio_clear_pin_interrupt_flag(ENC3_S1_PIN);
  }
  // ...and switches 0-3
  if(gpio_get_pin_interrupt_flag(SW0_PIN)) {
    process_sw(0);
    //    print_dbg( "  irq:sw:0" );
    gpio_clear_pin_interrupt_flag(SW0_PIN);
  }
  if(gpio_get_pin_interrupt_flag(SW1_PIN)) {
    process_sw(1);
    //    print_dbg( "  irq:sw:1" );
    gpio_clear_pin_interrupt_flag(SW1_PIN);
  }
  if(gpio_get_pin_interrupt_flag(SW2_PIN)) {
    process_sw(2);
    //    print_dbg( "  irq:sw:2" );
    gpio_clear_pin_interrupt_flag(SW2_PIN);
  }
  if(gpio_get_pin_interrupt_flag(SW3_PIN)) {
    process_sw(3);
    //    print_dbg( "  irq:sw:3" );
    gpio_clear_pin_interrupt_flag(SW3_PIN);
  }

}

//-----------------------------
//---- external function definitions

// register interrupts
void register_interrupts(void) {
  //  U8 i;

  /*
  // generate an interrupt when bfin HWAIT changes
  gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_PIN_CHANGE);

  // enable interrupts on encoder pins
  gpio_enable_pin_interrupt( ENC0_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC0_S1_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC1_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC1_S1_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC2_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC2_S1_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC3_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC3_S1_PIN,	GPIO_PIN_CHANGE);

  gpio_enable_pin_interrupt( SW0_PIN,	        GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( SW1_PIN,	        GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( SW2_PIN,	        GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( SW3_PIN,	        GPIO_PIN_CHANGE);
  */

  // FIXME: 
  //gpio_enable_pin_interrupt( SW_EDIT_PIN,	GPIO_PIN_CHANGE);
  
  /*
  // register IRQ for port A, 0-7
  INTC_register_interrupt( &irq_port0_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA00 / 8), AVR32_INTC_INT2 );
  
  // register IRQ for port A, 24-30
  INTC_register_interrupt( &irq_port0_line3, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA24 / 8), AVR32_INTC_INT2 );
  
  // register IRQ for port B, 0-7
  INTC_register_interrupt( &irq_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), AVR32_INTC_INT2 );
  
  // register IRQ for PDCA transfer
  INTC_register_interrupt(&irq_pdca, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1); 
  */

  // register TC interrupt
  INTC_register_interrupt(&irq_tc, APP_TC_IRQ, APP_TC_IRQ_PRIORITY);
}
