#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "config.h"
#include "usart.h"
#include "init.h"

//-------------------
//  static variables
// gpio map for oled-usart
static const gpio_map_t USART_SPI_GPIO_MAP = {
  {OLED_USART_SPI_MISO_PIN, OLED_USART_SPI_MISO_FUNCTION},
  {OLED_USART_SPI_MOSI_PIN, OLED_USART_SPI_MOSI_FUNCTION}
};

// spi options for oled-usart
static const usart_spi_options_t USART_SPI_OPTIONS = {
  .baudrate     = 60000,
  .charlength   = 8,
  .spimode      = 2, // clock starts high, sample on rising edge
  .channelmode  = USART_NORMAL_CHMODE
};

//--------------------
// static function declarations
static void init_clocks(void);
static void init_usart(void);

//----------------------
// static function definitions

// initialize clocks
void init_clocks(void) {
 // Switch main clock to external oscillator 0 (crystal).
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
}

// initialize usart
void init_usart(void) {
  // assign pins to usart
  gpio_enable_module(USART_SPI_GPIO_MAP,
		     sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));

  // nitialize usart in spi mode.
  usart_init_spi_master(OLED_USART_SPI, &USART_SPI_OPTIONS, FOSC0);
}

void init_avr(void) {
  //  U16 delay;
  init_clocks();

  // initialize interrupt vectors
  // necessary here because various peripheral drivers register interrupts internally
  INTC_init_interrupts();

  init_usart(); 
}
