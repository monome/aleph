#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "spi.h"
#include "eic.h"
#include "tc.h"
#include "intc.h"
#include "config.h"
//#include "protocol.h"
#include "init.h"

static void init_spi(void);
static void init_gpio(void);

void init_spi(void) {
  static const gpio_map_t BFIN_SPI_GPIO_MAP = {
    { BFIN_SPI_SCK_PIN, BFIN_SPI_SCK_FUNCTION },
    { BFIN_SPI_MISO_PIN, BFIN_SPI_MISO_FUNCTION },
    { BFIN_SPI_MOSI_PIN, BFIN_SPI_MOSI_FUNCTION },
    { BFIN_SPI_NPCS_PIN, BFIN_SPI_NPCS_FUNCTION }
  };

  spi_options_t spiOptions = {
    .reg          = BFIN_SPI_NPCS,
    .baudrate     = 20000000,
    .bits         = 8,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 1,
    .modfdis      = 1
  };

  // assign pins to SPI.
  gpio_enable_module(BFIN_SPI_GPIO_MAP,
		     sizeof(BFIN_SPI_GPIO_MAP) / sizeof(BFIN_SPI_GPIO_MAP[0]));

  // intialize as master
  spi_initMaster(BFIN_SPI, &spiOptions);

  // set selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(BFIN_SPI, 0, 0, 0);

  // enable SPI.
  spi_enable(BFIN_SPI);

  // intialize the chip register
  spi_setupChipReg(BFIN_SPI, &spiOptions, FOSC0);
}

void init_gpio(void) {
  // enable pulldown on bfin HWAIT line
  //// shit! not implemented... 
  // gpio_enable_pin_pull_down(BFIN_HWAIT_PIN);
  
  // enable pullup on bfin RESET line
  gpio_enable_pin_pull_up(BFIN_RESET_PIN);
}

void init_avr(void) {
  // initialize interrupt vectors
  // necessary here because various peripheral drivers register interrupts internally
  INTC_init_interrupts();
  init_spi();
  init_gpio(); 
}
