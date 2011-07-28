#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "spi.h"

// define SPI pins
#define BFIN_SPI                   (&AVR32_SPI0)
#define BFIN_SPI_NPCS              0
#define BFIN_SPI_SCK_PIN           AVR32_SPI0_SCK_0_0_PIN
#define BFIN_SPI_SCK_FUNCTION      AVR32_SPI0_SCK_0_0_FUNCTION
#define BFIN_SPI_MISO_PIN          AVR32_SPI0_MISO_0_0_PIN
#define BFIN_SPI_MISO_FUNCTION     AVR32_SPI0_MISO_0_0_FUNCTION
#define BFIN_SPI_MOSI_PIN          AVR32_SPI0_MOSI_0_0_PIN
#define BFIN_SPI_MOSI_FUNCTION     AVR32_SPI0_MOSI_0_0_FUNCTION
#define BFIN_SPI_NPCS_PIN          AVR32_SPI0_NPCS_0_0_PIN
#define BFIN_SPI_NPCS_FUNCTION     AVR32_SPI0_NPCS_0_0_FUNCTION

// static functions
static void init_spi( void );
//static void init_sys_clocks(void);

// static variables
static U64 counter = 0;
static U32 poog = 0;

int main(void) {

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	init_spi();

	while(1) {
		counter++;
		if (counter == (FOSC0 / 96)) {
			counter = 0;

			// flip LED1
			gpio_tgl_gpio_pin(LED1_GPIO);

			// write some poogs to the bfin spi
			poog = (poog + 1) % 16;
			spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
			spi_write(BFIN_SPI, (U8)poog);
			spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
		}
	}
}

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
	    .bits         = 8,
	    .spck_delay   = 0,
	    .trans_delay  = 0,
	    .stay_act     = 1,
	    .spi_mode     = 0,
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
