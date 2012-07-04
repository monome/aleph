/* init.c
 * oled test
 * aleph
 */

// asf
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
//#include "power_clocks_lib.h"
#include <flashc.h>
#include <gpio.h>
#include <pll.h>
#include <sysclk.h>
#include "tc.h"
#include "delay.h"
#include "usart.h"
// aleph
#include "config.h"
#include "init.h"

#define FASTCLOCK 1

//-------------------
//  static variables
// gpio map for oled-usart
static const gpio_map_t USART_SPI_GPIO_MAP = {
  {OLED_USART_SPI_SCK_PIN,  OLED_USART_SPI_SCK_FUNCTION },
  {OLED_USART_SPI_MISO_PIN, OLED_USART_SPI_MISO_FUNCTION},
  {OLED_USART_SPI_MOSI_PIN, OLED_USART_SPI_MOSI_FUNCTION},
  {OLED_USART_SPI_NSS_PIN,  OLED_USART_SPI_NSS_FUNCTION }
};

// SPI options for OLED
static const usart_spi_options_t USART_SPI_OPTIONS = {
  .baudrate     = 3300000, // seems about maximum
  .charlength   = 8,
  .spimode      = 3, // clock starts high, sample on rising edge
  .channelmode  = USART_NORMAL_CHMODE
};

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
  
//---------------------------
//-----  function definitions

// initialize clocks
void init_clocks(void) {

#if FASTCLOCK
  struct pll_config pllcfg;
#endif

  /* Initialize the synchronous clock system to the default configuration
   * set in conf_clock.h.
   * Note: all non-essential peripheral clocks are initially disabled.
   */
  sysclk_init();
  
  ///// re-enable desired peripheral clocks
  // interrupt controller
  sysclk_enable_pba_module(SYSCLK_INTC);
  // gpio
  sysclk_enable_pba_module(SYSCLK_GPIO);
  // usart
  sysclk_enable_pba_module(SYSCLK_USART2);
  // tc
  sysclk_enable_peripheral_clock(APP_TC);


  /*
   * Enable the PLL at PLL_OUTPUT_FREQ MHz and use
   * it as source of the main clock with CPU clock at half the
   * main clock frequency and PBA clock at 1/4 of the main clock
   * frequency.
   */
  /* Initialize the PLL configuration:
   * - PLL reference clock: OSC0
   * - div = 1,
   * - mul = PLL_OUTPUT_FREQ / BOARD_OSC0_HZ
   * => the PLL output frequency will be running at:
   * (mul / div) times the frequency of OSC0
   */

#if FASTCLOCK
  pll_config_init(&pllcfg, PLL_SRC_OSC0, 1,
		  PLL_OUTPUT_FREQ / BOARD_OSC0_HZ);
  // Configure and enable the PLL.
  pll_enable(&pllcfg, 0);
  /* Wait for the PLL to become locked (and thus ready to be used
   * as clock source).
   */
  pll_wait_for_lock(0);
  /* Set the prescalers applied to the main clock so that:
   * - CPU & HSB clock == PLL clock/2
   * - PBA clock == PLL clock/4
   * - PBB clock == PLL clock/2
   */
  sysclk_set_prescalers(1, 2, 1);

  /* Before changing the main clock source, set the appropriate
   * FLASH wait state depending on the new HSB bus frequency.
   */
  flash_set_bus_freq(PLL_OUTPUT_FREQ / 2);

  // Switch the main clock to PLL0.
  sysclk_set_source(SYSCLK_SRC_PLL0);
  //wait_for_switches();

  // Switch main clock to external oscillator 0 (crystal).
  ///  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
#endif

  // intitialize millisecond delay engine
  delay_init(sysclk_get_cpu_hz());
}

// initialize usart
void init_usart(void) {
  // Assign GPIO to SPI.
  gpio_enable_module(USART_SPI_GPIO_MAP,
		     sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));
  // Initialize USART in SPI mode.
  usart_init_spi_master(OLED_USART_SPI, &USART_SPI_OPTIONS, FOSC0);
  delay_ms(10);
}

void init_gpio(void) {

  gpio_enable_pin_pull_up(ENC0_S0_PIN);
  gpio_enable_pin_pull_up(ENC0_S1_PIN);
  
  /*
    gpio_enable_pin_pull_up(ENC1_S0_PIN);
    gpio_enable_pin_pull_up(ENC1_S1_PIN);
    gpio_enable_pin_pull_up(ENC2_S0_PIN);
    gpio_enable_pin_pull_up(ENC2_S1_PIN);
    gpio_enable_pin_pull_up(ENC3_S0_PIN);
    gpio_enable_pin_pull_up(ENC3_S1_PIN);
  */
}

// intialize timer/counter
void init_tc(volatile avr32_tc_t *tc)
{

  // Initialize the timer/counter.
  tc_init_waveform(tc, &waveform_opt);

  // set timer compare trigger.
  // we want it to overflow and generate an interrupt every 10 ms
  // so (1 / fPBA / 128) * RC = 10,
  // so RC = fPBA / 128 / 1000

  tc_write_rc(tc, APP_TC_CHANNEL, (sysclk_get_pba_hz() / 128 / 1000));
  // configure the timer interrupt
  tc_configure_interrupts(tc, APP_TC_CHANNEL, &tc_interrupt);
  // Start the timer/counter.
  tc_start(tc, APP_TC_CHANNEL);
}

/*
// high-level init function
void init_avr(void) {
avr32_tc_t tc;

// clocks
init_clocks();
// GPIO
init_gpio();
// USARTs
init_usart();
// timer/counter
init_tc(&tc);
}
*/
