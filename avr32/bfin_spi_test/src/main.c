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

//////////// typedefs

// encoder struct
typedef struct {
	unsigned char pos_now, pos_old;

	int value;
	int min, max;
	int step;

	int event_id;
	int event_type;
} enc_t;

typedef struct {
	unsigned char state;

	int mode; // normal, toggle
	int toggle_state;
	int value_on;
	int value_off;
	int send_on;
	int send_off;

	int event_id;
	int event_type;
} sw_t;

#define SW_MODE_NORMAL 0
#define SW_MODE_TOGGLE 1

// event types: (enum?) control = avr32, param = bf, preset
#define EVENT_CONTROL	0
#define EVENT_PARAM		1
#define EVENT_PRESET	2

//////////// static vars

// for encoder position tracking
static const int enc_map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };

static enc_t enc[4];
static sw_t sw[4];
static unsigned int param_delivery[3];

// array of pointers for local control variables.
// possibly should be turned into a class with public functions
static int *control_id[] = {
		&enc[0].value,
		&enc[0].min,
		&enc[0].max,
		&enc[0].step,
		&enc[0].event_id,
		&enc[0].event_type
};


//////////// function declarations

static void register_interrupts( void );
static void HandleEncInterrupt( U8 pin );
static void HandleSwInterrupt( U8 pin );
static inline void checkEncInterruptPin(const U8 pin);
static inline void checkSwInterruptPin(const U8 pin);


//////////// interrupts
__attribute__((__interrupt__))
static void int_handler_port0_line0( void )
{
	gpio_tgl_gpio_pin(LED0_GPIO);

	checkEncInterruptPin(CON_ENC0_S0);
	checkEncInterruptPin(CON_ENC0_S1);
}

__attribute__((__interrupt__))
static void int_handler_sw( void )
{
	gpio_tgl_gpio_pin(LED1_GPIO);

	checkSwInterruptPin(CON_SW0);
	checkSwInterruptPin(CON_SW1);
	checkSwInterruptPin(CON_SW2);
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int main(void) {
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	init_spi();
	init_enc();

	gpio_enable_pin_pull_up(CON_SW0);
	gpio_enable_pin_pull_up(CON_SW1);
	gpio_enable_pin_pull_up(CON_SW2);


	enc[0].pos_old = gpio_get_pin_value(CON_ENC0_S0) + (gpio_get_pin_value(CON_ENC0_S1) << 1);
	enc[0].min = 0;
	enc[0].max = 1023;
	enc[0].event_id = 0;
	enc[0].event_type = EVENT_PARAM;

	sw[0].state = 0;
	sw[0].mode = SW_MODE_NORMAL;
	sw[0].value_on = 255;
	sw[0].value_off = 0;
	sw[0].send_on = 1;
	sw[0].send_off = 1;
	sw[0].event_id = 1;
	sw[0].event_type = EVENT_PARAM;

	Disable_global_interrupt();
	register_interrupts();
	Enable_global_interrupt();

	while(1) {
		;;
	}
}



//////////// function definitions

// encoder interrupt
void HandleEncInterrupt( U8 pin )
{
	enc[0].pos_now = gpio_get_pin_value(CON_ENC0_S0) + (gpio_get_pin_value(CON_ENC0_S1) << 1);
	if(enc[0].pos_now != enc[0].pos_old) {
		enc[0].value += enc_map[enc[0].pos_old][enc[0].pos_now];
		if(enc[0].value < enc[0].min) enc[0].value = enc[0].min;
		else if(enc[0].value > enc[0].max) enc[0].value = enc[0].max;

		if(enc[0].event_type == EVENT_CONTROL) {
			*control_id[enc[0].event_id] = enc[0].value;
		}
		else if (enc[0].event_type == EVENT_PARAM) {
			param_delivery[0] = P_SET_PARAM_COMMAND_WORD(P_PARAM_COM_SETI,
					enc[0].event_id);
			param_delivery[1] = P_SET_PARAM_DATA_WORD_H(enc[0].value);
			param_delivery[2] = P_SET_PARAM_DATA_WORD_L(enc[0].value);

			spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
			spi_write(BFIN_SPI, param_delivery[0]);
			spi_write(BFIN_SPI, param_delivery[1]);
			spi_write(BFIN_SPI, param_delivery[2]);
			spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
		}
		// no EVENT_PRESET for encoders
	}

	enc[0].pos_old = enc[0].pos_now;
}

// sw interrupt
void HandleSwInterrupt( U8 pin )
{
	int v, nosend = 0;

	switch(pin) {
	case CON_SW0:
		if(sw[0].mode == SW_MODE_NORMAL)
			sw[0].state = gpio_get_pin_value(CON_ENC0_S0);
		else {
			if(gpio_get_pin_value(CON_ENC0_S0)) {
				sw[0].state ^= 1;
			}
			else nosend = 1;
		}

		if(sw[0].state) {
			v = sw[0].value_on;
			if(!sw[0].send_on)
				nosend++;
		}
		else {
			v = sw[0].value_off;
			if(!sw[0].send_off)
				nosend++;
		}

		if(!nosend) {
			if(sw[0].event_type == EVENT_CONTROL) {
				*control_id[sw[0].event_id] = v;
			}
			else if(sw[0].event_type == EVENT_PARAM) {
				param_delivery[0] = P_SET_PARAM_COMMAND_WORD(P_PARAM_COM_SETI,
					sw[0].event_id);
				param_delivery[1] = P_SET_PARAM_DATA_WORD_H(v);
				param_delivery[2] = P_SET_PARAM_DATA_WORD_L(v);

				spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
				spi_write(BFIN_SPI, param_delivery[0]);
				spi_write(BFIN_SPI, param_delivery[1]);
				spi_write(BFIN_SPI, param_delivery[2]);
				spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
			}
			else if(sw[0].event_type == EVENT_PRESET) {
				// recall preset
			}
		}
	}
}


// check pin for interrupt, call handler if so
static inline void checkEncInterruptPin(const U8 pin)
{
	if(gpio_get_pin_interrupt_flag(pin))
	{
		HandleEncInterrupt( pin );
		gpio_clear_pin_interrupt_flag(pin);
	}
}

// check pin for interrupt, call handler if so
static inline void checkSwInterruptPin(const U8 pin)
{
	if(gpio_get_pin_interrupt_flag(pin))
	{
		HandleSwInterrupt( pin );
		gpio_clear_pin_interrupt_flag(pin);
	}
}

// register interrupts for needed pins
static void register_interrupts( void )
{
	gpio_enable_pin_interrupt( CON_ENC0_S0,	GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt( CON_ENC0_S1,	GPIO_PIN_CHANGE);

	INTC_register_interrupt( &int_handler_port0_line0, AVR32_GPIO_IRQ_0, AVR32_INTC_INT1 );

	gpio_enable_pin_interrupt( CON_SW0,	GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt( CON_SW1,	GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt( CON_SW2,	GPIO_PIN_CHANGE);

	INTC_register_interrupt( &int_handler_sw, AVR32_GPIO_IRQ_0 + (CON_SW0/8), AVR32_INTC_INT1 );
	INTC_register_interrupt( &int_handler_sw, AVR32_GPIO_IRQ_0 + (CON_SW1/8), AVR32_INTC_INT1 );
	INTC_register_interrupt( &int_handler_sw, AVR32_GPIO_IRQ_0 + (CON_SW2/8), AVR32_INTC_INT1 );
}
