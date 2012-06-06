/* init.c
 * oled test
 * aleph
 */

// asf
#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "delay.h"
#include "usart.h"
// aleph
#include "config.h"
#include "init.h"

//-------------------
//  static variables
// gpio map for oled-usart
static const gpio_map_t USART_SPI_GPIO_MAP = {
  {OLED_USART_SPI_SCK_PIN,  OLED_USART_SPI_SCK_FUNCTION },
  {OLED_USART_SPI_MISO_PIN, OLED_USART_SPI_MISO_FUNCTION},
  {OLED_USART_SPI_MOSI_PIN, OLED_USART_SPI_MOSI_FUNCTION},
  {OLED_USART_SPI_NSS_PIN,  OLED_USART_SPI_NSS_FUNCTION }
};

static const usart_spi_options_t USART_SPI_OPTIONS = {
  .baudrate     = 3300000,
  .charlength   = 8,
  .spimode      = 3, // clock starts high, sample on rising edge
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
 delay_init(FOSC0);
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

void init_avr(void) {
  init_clocks();
  init_usart(); 
}
