// ASF
//#include "compiler.h"
/* #include "delay.h" */
/* #include "gpio.h" */
/* #include "intc.h" */
/* #include "pdca.h" */
#include "print_funcs.h"
/* #include "sd_mmc_spi.h" */
/* #include "tc.h" */
/* #include "usart.h" */
// aleph
//#include "aleph_board.h"
#include "bfin.h"
//#include "conf_tc_irq.h"
#include "encoders.h"
#include "events.h"
#include "event_types.h"
#include "filesystem.h"
#include "global.h"
#include "interrupts.h"
#include "serial.h"
#include "switches.h"
#include "timers.h"
#include "types.h"

//#define UI_IRQ_PRIORITY AVR32_INTC_INT2

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

// irq for pdca (sdcard)
// __attribute__((__interrupt__))
static void irq_pdca(void);

// irq for app timer
// __attribute__((__interrupt__))
static void irq_tc(void);

// irq for PA24-PA31
// __attribute__((__interrupt__))
static void irq_port0_line3(void);

// irq for PB00-PB07
// __attribute__((__interrupt__))
static void irq_port1_line0(void);

// irq for PB08-PB15
// __attribute__((__interrupt__))
static void irq_port1_line1(void);

// irq for PB16-PB23
// __attribute__((__interrupt__))
static void irq_port1_line2(void);

// irq for PB24-PB31
// __attribute__((__interrupt__))
static void irq_port1_line3(void);

// irq for uart
// __attribute__((__interrupt__))
static void irq_usart(void);


//---------------------------------
//----- static function definitions
// __attribute__((__interrupt__))
static void irq_pdca(void) {
#if 1
#else
  // Disable all interrupts.
  //  Disable_global_interrupt();
  cpu_irq_disable();
  // Disable interrupt channel.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
  //unselects the SD/MMC memory.
  sd_mmc_spi_read_close_PDCA();
  //.... example has a 5000 clock gimpy delay here.
  // using delay_us instead
  delay_ms(10);
  //  delay_ms(2);
  // Disable unnecessary channel
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);
  // Enable all interrupts.
  cpu_irq_enable();
  //  Enable_global_interrupt();
  //  print_dbg("\r\n handled PDCA interrupt. \r\n");
  fsEndTransfer = true;
#endif
}

// timer irq
// __attribute__((__interrupt__))
static void irq_tc(void) {
#if 1
#else
  tcTicks++;
  // overflow control
  if(tcTicks > tcMax) { 
    tcTicks = 0;
    tcOverflow = 1;
  } else {
    tcOverflow = 0;
  }
  process_timers();
  // clear interrupt flag by reading timer SR
  tc_read_sr(APP_TC, APP_TC_CHANNEL);
#endif
}


/* // interrupt handler for PA00-PA07 */
/* // __attribute__((__interrupt__)) */
/* static void irq_port0_line0(void) {
#if 1
#else */
/*   if(gpio_get_pin_interrupt_flag(AVR32_PIN_PA04)) { */
/*     gpio_clear_pin_interrupt_flag(AVR32_PIN_PA04); */
/*       gpio_toggle_pin(LED_MODE_PIN); */
/*   } */
/* } */

// interrupt handler for PA23-PA30
// __attribute__((__interrupt__))
static void irq_port0_line3(void) {
#if 1
#else
  //  print_dbg("\r\n interrupt on port0_line3");
  //SW_F0
  if(gpio_get_pin_interrupt_flag(SW0_PIN)) {
    gpio_clear_pin_interrupt_flag(SW0_PIN);
    /// process_sw() will post an event, which calls cpu_irq_disable().
    /// apparently, this also clears the GPIO interrupt flags
    /// so clear the flag first to avoid triggering an infinite series of interrupts.
    /// this might be problematic if we were expecting faster interrupts from switches,
    /// but hardware pre-filtering should preclude this.
    process_sw(0);
  }
  // SW_F1
  if(gpio_get_pin_interrupt_flag(SW1_PIN)) {
    gpio_clear_pin_interrupt_flag(SW1_PIN);
    process_sw(1);
  }
  // SW_F2
  if(gpio_get_pin_interrupt_flag(SW2_PIN)) {
    gpio_clear_pin_interrupt_flag(SW2_PIN);
    process_sw(2);
  }
 
  // SW_F3
  if(gpio_get_pin_interrupt_flag(SW3_PIN)) {
    gpio_clear_pin_interrupt_flag(SW3_PIN);
    process_sw(3);
  }
  // SW_MODE
  if(gpio_get_pin_interrupt_flag(SW_MODE_PIN)) {
    gpio_clear_pin_interrupt_flag(SW_MODE_PIN);
    process_sw(4);
  }
#endif
}

// interrupt handler for PB00-PB07
// __attribute__((__interrupt__))
static void irq_port1_line0(void) {
#if 1
#else
  // print_dbg("\r\b\interrupt on PB00-PB07.");
  // ENC0_0
  if(gpio_get_pin_interrupt_flag(ENC0_S0_PIN)) {
    process_enc(0);
    gpio_clear_pin_interrupt_flag(ENC0_S0_PIN);
  }  
  // ENC0_1
  if(gpio_get_pin_interrupt_flag(ENC0_S1_PIN)) {
    process_enc(0);
    gpio_clear_pin_interrupt_flag(ENC0_S1_PIN);
  }
  // ENC1_0
  if(gpio_get_pin_interrupt_flag(ENC1_S0_PIN)) {
    process_enc(1);
    gpio_clear_pin_interrupt_flag(ENC1_S0_PIN);
  }  
  // ENC1_1
  if(gpio_get_pin_interrupt_flag(ENC1_S1_PIN)) {
    process_enc(1);
    gpio_clear_pin_interrupt_flag(ENC1_S1_PIN);
  }
  // ENC2_0
  if(gpio_get_pin_interrupt_flag(ENC2_S0_PIN)) {
    process_enc(2);
    gpio_clear_pin_interrupt_flag(ENC2_S0_PIN);
  }  
  // ENC2_1
  if(gpio_get_pin_interrupt_flag(ENC2_S1_PIN)) {
    process_enc(2);
    gpio_clear_pin_interrupt_flag(ENC2_S1_PIN);
  }
#endif
}

// interrupt handler for PB08-PB15
// __attribute__((__interrupt__))
static void irq_port1_line1(void) {
#if 1
#else
  //    print_dbg("\r\b\interrupt on PB08-PB15.");
  // ENC3_0
  if(gpio_get_pin_interrupt_flag(ENC3_S0_PIN)) {
    process_enc(3);
    gpio_clear_pin_interrupt_flag(ENC3_S0_PIN);
  }  
  // ENC3_1
  if(gpio_get_pin_interrupt_flag(ENC3_S1_PIN)) {
    process_enc(3);
    gpio_clear_pin_interrupt_flag(ENC3_S1_PIN);
  }

#endif
}

// interrupt handler for PB16-PB23
// __attribute__((__interrupt__))
static void irq_port1_line2(void) {
#if 1
#else
  //  print_dbg("\r\n interrupt on pb16-pb23 : ");
  //SW_POWER
  if(gpio_get_pin_interrupt_flag(SW_POWER_PIN)) {
    gpio_clear_pin_interrupt_flag(SW_POWER_PIN);
    process_sw(5);
  }
#endif
}

// interrupt handler for PB24-PB31
// __attribute__((__interrupt__))
static void irq_port1_line3(void) {
#if 1
#else
  //  print_dbg("\r\n irq_port1_line3");
  if(gpio_get_pin_interrupt_flag(FS0_PIN)) {
    gpio_clear_pin_interrupt_flag(FS0_PIN);
    process_sw(6);
  }
  if(gpio_get_pin_interrupt_flag(FS1_PIN)) {
    gpio_clear_pin_interrupt_flag(FS1_PIN);
    process_sw(7);
  }
#endif
}

// interrupt handler for uart
// __attribute__((__interrupt__))
static void irq_usart(void) {
#if 1
#else
  serial_process();
  // usart_write_char(FTDI_USART,c);
  // print_dbg("\r\nusb cable change.");
  gpio_toggle_pin(LED_MODE_PIN);
#endif
}

//-----------------------------
//---- external function definitions

// register interrupts
void register_interrupts(void) {
#if 1
#else
  // enable interrupts on GPIO inputs

  // BFIN_HWAIT
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
  gpio_enable_pin_interrupt( SW_POWER_PIN,	GPIO_PIN_CHANGE);
 
  // PA24 - PA31
  INTC_register_interrupt( &irq_port0_line3, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA24 / 8), UI_IRQ_PRIORITY);

  // PB00 - PB07
  INTC_register_interrupt( &irq_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), UI_IRQ_PRIORITY);

  // PB08 - PB15
  INTC_register_interrupt( &irq_port1_line1, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB08 / 8), UI_IRQ_PRIORITY);

  // PB16 - PB23
  INTC_register_interrupt( &irq_port1_line2, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB16 / 8), UI_IRQ_PRIORITY);

  // PB24 - PB31
  INTC_register_interrupt( &irq_port1_line3, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB24 / 8), UI_IRQ_PRIORITY);

  // register IRQ for PDCA transfer
  INTC_register_interrupt(&irq_pdca, AVR32_PDCA_IRQ_0, SYS_IRQ_PRIORITY);

  // register TC interrupt
  INTC_register_interrupt(&irq_tc, APP_TC_IRQ, APP_TC_IRQ_PRIORITY);

  // register uart interrupt
  INTC_register_interrupt(&irq_usart, AVR32_USART0_IRQ, UI_IRQ_PRIORITY);
#endif
}
