/* init.c
   aleph-avr32
   
   low-level peripheral  initialization.
*/

//ASF
#include <flashc.h>
#include <pll.h>
#include <sysclk.h>
#include <avr32/io.h>

#include "compiler.h"
#include "conf_sd_mmc_spi.h"
#include "util.h"
#include "gpio.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "spi.h"
#include "tc.h"
#include "usart.h"
#include "twi.h"

// aleph
//#include "conf_aleph.h"
#include "aleph_board.h"
#include "conf_tc_irq.h"
#include "init.h"
#include "i2c.h"
//===========================
//==== static variables
// ...

twi_options_t opt;
twi_slave_fct_t twi_slave_fct;



//===================================
//==== external functions

// initialize non-peripheral GPIO
void init_gpio(void) {
  
  gpio_enable_pin_pull_up(ENC0_S0_PIN);
  gpio_enable_pin_pull_up(ENC0_S1_PIN);
  gpio_enable_pin_pull_up(ENC1_S0_PIN);
  gpio_enable_pin_pull_up(ENC1_S1_PIN);
  gpio_enable_pin_pull_up(ENC2_S0_PIN);
  gpio_enable_pin_pull_up(ENC2_S1_PIN);
  gpio_enable_pin_pull_up(ENC3_S0_PIN);
  gpio_enable_pin_pull_up(ENC3_S1_PIN);

#if 0
  gpio_enable_pin_pull_up(SW0_PIN);
  gpio_enable_pin_pull_up(SW1_PIN);
  gpio_enable_pin_pull_up(SW2_PIN);
  gpio_enable_pin_pull_up(SW3_PIN);
  gpio_enable_pin_pull_up(SW_MODE_PIN);
#endif

  gpio_enable_pin_pull_up(SW_POWER_PIN);

  /// trying this...
  /* gpio_enable_pin_glitch_filter(SW0_PIN); */
  /* gpio_enable_pin_glitch_filter(SW1_PIN); */
  /* gpio_enable_pin_glitch_filter(SW2_PIN); */
  /* gpio_enable_pin_glitch_filter(SW3_PIN); */
  gpio_enable_pin_glitch_filter(SW_MODE_PIN); 
}

// initialize application timer
extern void init_tc (volatile avr32_tc_t *tc) {
  // waveform options
  static const tc_waveform_opt_t waveform_opt = {
    .channel  = APP_TC_CHANNEL,  // channel
    .bswtrg   = TC_EVT_EFFECT_NOOP, // software trigger action on TIOB
    .beevt    = TC_EVT_EFFECT_NOOP, // external event action
    .bcpc     = TC_EVT_EFFECT_NOOP, // rc compare action
    .bcpb     = TC_EVT_EFFECT_NOOP, // rb compare
    .aswtrg   = TC_EVT_EFFECT_NOOP, // soft trig on TIOA
    .aeevt    = TC_EVT_EFFECT_NOOP, // etc
    .acpc     = TC_EVT_EFFECT_NOOP,
    .acpa     = TC_EVT_EFFECT_NOOP,
    // Waveform selection: Up mode with automatic trigger(reset) on RC compare.
    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
    .enetrg   = false,             // external event trig
    .eevt     = 0,                 // extern event select
    .eevtedg  = TC_SEL_NO_EDGE,    // extern event edge
    .cpcdis   = false,             // counter disable when rc compare
    .cpcstop  = false,            // counter stopped when rc compare
    .burst    = false,
    .clki     = false,
    // Internal source clock 5, connected to fPBA / 128.
    .tcclks   = TC_CLOCK_SOURCE_TC5
  };

  // Options for enabling TC interrupts
  static const tc_interrupt_t tc_interrupt = {
    .etrgs = 0,
    .ldrbs = 0,
    .ldras = 0,
    .cpcs  = 1, // Enable interrupt on RC compare alone
    .cpbs  = 0,
    .cpas  = 0,
    .lovrs = 0,
    .covfs = 0
  };
  // Initialize the timer/counter.
  tc_init_waveform(tc, &waveform_opt);

  // set timer compare trigger.
  // we want it to overflow and generate an interrupt every 1 ms
  // so (1 / fPBA / 128) * RC = 0.001
  // so RC = fPBA / 128 / 1000
  tc_write_rc(tc, APP_TC_CHANNEL, (FPBA_HZ / 128 / 1000));
  // configure the timer interrupt
  tc_configure_interrupts(tc, APP_TC_CHANNEL, &tc_interrupt);
  // Start the timer/counter.
  tc_start(tc, APP_TC_CHANNEL);
}


// initialize usb USARTy
void init_ftdi_usart (void) {
  // GPIO map for USART.
  static const gpio_map_t FTDI_USART_GPIO_MAP = {
    { FTDI_USART_RX_PIN, FTDI_USART_RX_FUNCTION },
    { FTDI_USART_TX_PIN, FTDI_USART_TX_FUNCTION }
  };
  
  // Options for USART.
  static const usart_options_t FTDI_USART_OPTIONS = {
    .baudrate = FTDI_USART_BAUDRATE,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  // Set up GPIO for FTDI_USART
  gpio_enable_module(FTDI_USART_GPIO_MAP,
                     sizeof(FTDI_USART_GPIO_MAP) / sizeof(FTDI_USART_GPIO_MAP[0]));

  // Initialize in RS232 mode.
  usart_init_rs232(FTDI_USART, &FTDI_USART_OPTIONS, FPBA_HZ);
}


// initialize i2c
void init_i2c(void) {
  int status;
  /* clock div calculation in the twi is grossly inaccurate,
     so we manually re-set the divisors to get as close to 400k as possible.
     there is also some very weird offset going on, 
     so these values are experimentally determined,
     and don't really match what one would expect from the datasheet :S
   */

  u32 c_lh_div = 63;
  u32 ckdiv = 0;

  // TWI/I2C GPIO map
  static const gpio_map_t TWI_GPIO_MAP = {
    { TWI_DATA_PIN, TWI_DATA_FUNCTION },
    { TWI_CLOCK_PIN, TWI_CLOCK_FUNCTION }
  };
  gpio_enable_module(TWI_GPIO_MAP, sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));

  // options settings
  opt.pba_hz = FPBA_HZ;
  opt.speed = TWI_SPEED;
  // FIXME: make this an argument
  opt.chip = 100;

    // initialize TWI driver with options
  twi_slave_fct.rx = &i2c_slave_rx;
  twi_slave_fct.tx = &i2c_slave_tx;
  twi_slave_fct.stop = &i2c_slave_stop;
  status = twi_slave_init(&AVR32_TWI, &opt, &twi_slave_fct );

  // overwrite clock settings with 400k
  AVR32_TWI.cwgr = ((c_lh_div << AVR32_TWI_CWGR_CLDIV_OFFSET) |
      (c_lh_div << AVR32_TWI_CWGR_CHDIV_OFFSET) |
      (ckdiv << AVR32_TWI_CWGR_CKDIV_OFFSET));
}


// intialize TWI in slave mode
void init_i2c_slave(void) {
  int status;

  // FIXME: make this an argument
  opt.chip = 100;

  // // initialize TWI driver with options
  // twi_slave_fct.rx = &i2c_slave_rx;
  // twi_slave_fct.tx = &i2c_slave_tx;
  // twi_slave_fct.stop = &i2c_slave_stop;
  status = twi_slave_init(&AVR32_TWI, &opt, &twi_slave_fct );

  // // check init result
  // if (status == TWI_SUCCESS)
  //   {
  //     // display test result to user
  //     print_dbg("\r\ntwi slave start:\tPASS");
  //   }
  // else
  //   {
  //     // display test result to user
  //     print_dbg("\r\ntwi slave start:\tFAIL");
  //   }
}

// intialize I2C in master mode
void init_i2c_master(void) {
  int status;

  /* clock div calculation in the twi is grossly inaccurate,
     so we manually re-set the divisors to get as close to 400k as possible.
     there is also some very weird offset going on, 
     so these values are experimentally determined,
     and don't really match what one would expect from the datasheet :S
   */

  u32 c_lh_div = 63;
  u32 ckdiv = 0;
  
  status = twi_master_init(&AVR32_TWI, &opt);
  // AVR32_TWI.cwgr = ((c_lh_div << AVR32_TWI_CWGR_CLDIV_OFFSET) |
		// 	(c_lh_div << AVR32_TWI_CWGR_CHDIV_OFFSET) |
		// 	(ckdiv << AVR32_TWI_CWGR_CKDIV_OFFSET));
  // check init result
  // if (status == TWI_SUCCESS)
  //   {
  //     // display test result to user
  //     print_dbg("\r\ntwi master start:\tPASS");
  //   }
  // else
  //   {
  //     // display test result to user
  //     print_dbg("\r\ntwi master start:\tFAIL");
  //   }

    // overwrite clock settings with 400k
  AVR32_TWI.cwgr = ((c_lh_div << AVR32_TWI_CWGR_CLDIV_OFFSET) |
      (c_lh_div << AVR32_TWI_CWGR_CHDIV_OFFSET) |
      (ckdiv << AVR32_TWI_CWGR_CKDIV_OFFSET));
}
