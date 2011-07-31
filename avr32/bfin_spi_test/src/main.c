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
