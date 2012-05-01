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
#include "protocol.h"
#include "init.h"

// static vars
void init_spi(void) {
	static const gpio_map_t BFIN_SPI_GPIO_MAP = {
		{ BFIN_SPI_SCK_PIN, BFIN_SPI_SCK_FUNCTION },
		{ BFIN_SPI_MISO_PIN, BFIN_SPI_MISO_FUNCTION },
		{ BFIN_SPI_MOSI_PIN, BFIN_SPI_MOSI_FUNCTION },
		{ BFIN_SPI_NPCS_PIN, BFIN_SPI_NPCS_FUNCTION }
	};

	spi_options_t spiOptions = {
		.reg          = BFIN_SPI_NPCS,
		.baudrate     = 1000000,
	    .bits         = 16,
	    .spck_delay   = 0,
	    .trans_delay  = 10, // TODO: calculate correct transfer delay
	    .stay_act     = 1,
	    .spi_mode     = 1,
	    .modfdis      = 1
	  };

  // Assign I/Os to SPI.
  gpio_enable_module(BFIN_SPI_GPIO_MAP,
					 sizeof(BFIN_SPI_GPIO_MAP) / sizeof(BFIN_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(BFIN_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(BFIN_SPI, 0, 0, 0);

  // Enable SPI.
  spi_enable(BFIN_SPI);

  // intialize the chip register
  spi_setupChipReg(BFIN_SPI, &spiOptions, FOSC0);
}


void init_control_pins( void )
{
	gpio_enable_pin_pull_up(CON_ENC0_S0);
	gpio_enable_pin_pull_up(CON_ENC0_S1);
	gpio_enable_pin_pull_up(CON_ENC1_S0);
	gpio_enable_pin_pull_up(CON_ENC1_S1);

	gpio_enable_pin_pull_up(CON_SW0);
	gpio_enable_pin_pull_up(CON_SW1);
	gpio_enable_pin_pull_up(CON_SW2);
}
