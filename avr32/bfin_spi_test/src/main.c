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

typedef struct {
	unsigned char pos_now, pos_old;

	int value;
	int min, max;
	float step, multiply;
} enc_t;

static enc_t enc[4];
static unsigned int param_delivery[3];

const int enc_map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };


//---------------- static variables


//---------------  static function prototypes
static void init_spi( void );
void HandleEncInterrupt( U8 pin );
static inline void checkEncInterruptPin(const U8 pin);
static void register_interrupts( void );
static void init_enc( void );


//---------------- ints
__attribute__((__interrupt__))
static void int_handler_port0_line0( void )
{
	gpio_tgl_gpio_pin(LED0_GPIO);

	checkEncInterruptPin(CON_ENC0_S0);
	checkEncInterruptPin(CON_ENC0_S1);
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int main(void) {

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	init_spi();

	init_enc();
	Disable_global_interrupt();
	register_interrupts();
	Enable_global_interrupt();


	while(1) {
		;;
	}
}



//--------------- static function definitions

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
	    .trans_delay  = 100, // TODO: calculate correct transfer delay
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

void HandleEncInterrupt( U8 pin )
{
	enc[0].pos_now = gpio_get_pin_value(CON_ENC0_S0) + (gpio_get_pin_value(CON_ENC0_S1) << 1);
	if(enc[0].pos_now != enc[0].pos_old) {
		enc[0].value += enc_map[enc[0].pos_old][enc[0].pos_now];
		if(enc[0].value < enc[0].min) enc[0].value = enc[0].min;
		else if(enc[0].value > enc[0].max) enc[0].value = enc[0].max;

		param_delivery[0] = P_SET_PARAM_COMMAND_WORD(P_PARAM_COM_SETI, 0);
		param_delivery[1] = P_SET_PARAM_DATA_WORD_H(enc[0].value);
		param_delivery[2] = P_SET_PARAM_DATA_WORD_L(enc[0].value);

		spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
		spi_write(BFIN_SPI, param_delivery[0]);
		spi_write(BFIN_SPI, param_delivery[1]);
		spi_write(BFIN_SPI, param_delivery[2]);
		spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
	}

	enc[0].pos_old = enc[0].pos_now;
}

static inline void checkEncInterruptPin(const U8 pin)
{
	if(gpio_get_pin_interrupt_flag(pin))
	{
		HandleEncInterrupt( pin );
		gpio_clear_pin_interrupt_flag(pin);
	}
}

static void register_interrupts( void )
{
	gpio_enable_pin_interrupt( CON_ENC0_S0,	GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt( CON_ENC0_S1,	GPIO_PIN_CHANGE);

	INTC_register_interrupt( &int_handler_port0_line0, AVR32_GPIO_IRQ_0, AVR32_INTC_INT1 );
}
static void init_enc( void )
{
	gpio_enable_pin_pull_up(CON_ENC0_S0);
	gpio_enable_pin_pull_up(CON_ENC0_S1);

	enc[0].pos_old = gpio_get_pin_value(CON_ENC0_S0) + (gpio_get_pin_value(CON_ENC0_S1) << 1);
	enc[0].min = 0;
	enc[0].max = 1023;
}
