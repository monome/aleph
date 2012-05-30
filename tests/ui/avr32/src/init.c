#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
//#include "spi.h"
//#include "eic.h"
//#include "tc.h"
#include "intc.h"
#include "config.h"
#include "usart.h"
//#include "protocol.h"
#include "init.h"

static void init_usart(void);

static const gpio_map_t USART_SPI_GPIO_MAP = {
  {OLED_USART_SPI_MISO_PIN, OLED_USART_SPI_MISO_FUNCTION},
  {OLED_USART_SPI_MOSI_PIN, OLED_USART_SPI_MOSI_FUNCTION}
};


static const usart_spi_options_t USART_SPI_OPTIONS = {
  .baudrate     = 60000,
  .charlength   = 8,
  .spimode      = 0,
    .channelmode  = USART_NORMAL_CHMODE
};

void init_usart(void) {
  // Assign GPIO to SPI.
  gpio_enable_module(USART_SPI_GPIO_MAP,
		     sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));

  // Initialize USART in SPI mode.
  usart_init_spi_master(OLED_USART_SPI, &USART_SPI_OPTIONS, FOSC0);
}

void init_avr(void) {
  // initialize interrupt vectors
  // necessary here because various peripheral drivers register interrupts internally
  INTC_init_interrupts();
  init_usart(); 
}
