
/*! \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the FLASHC software driver.
 *
 * It also comes bundled with an example. This example demonstrates flash read /
 * write data accesses, using a flash block as an NVRAM, located either in the
 * flash array or in the User page.
 *
 * Operating mode of the example:
 *   -# After reset, the NVRAM variables are displayed on the USART link.
 *   -# The NVRAM is cleared (all bytes are set to 0x00).
 *   -# All NVRAM variables are written with incrementing nibbles, starting from
 *      0x0.
 *   -# The user can reset or power-cycle the board to check the
 *      non-volatileness of the NVRAM.
 *
 * \section files Main Files
 *   - flashc.c: FLASHC driver;
 *   - flashc.h: FLASHC driver header file;
 *   - flash_example.c: flash access example application.
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with a FLASHC module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 *   - EVK1100, EVK1101, UC3C_EK or EVK1104 or EVK1105 evaluation kit;
 *     Note: this example doesn't work on AT32UC3A0512 before revJ.
 *     Check the application note AVR32749 on www.atmel.com for a workaround.
 *   - CPU clock:
 *        -- 12 MHz : EVK1100, EVK1101, EVK1104, EVK1105 evaluation kits
 *        -- 16 MHz : UC3C_EK
 *   - USART1 (on EVK1100 or EVK1101) connected to a PC serial port via a
 *     standard RS232 DB9 cable, or USART0 (on EVK1105) or USART1 (on EVK1104)
 *     or USART2 (on UC3C_EK) abstracted with a USB CDC connection to a PC;
 *   - PC terminal settings:
 *     - 57600 bps,
 *     - 8 data bits,
 *     - no parity bit,
 *     - 1 stop bit,
 *     - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */


#include "compiler.h"
#include "print_funcs.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "flashc.h"


//! Structure type containing variables to store in NVRAM using a specific
//! memory map.
typedef const struct {
	uint8_t  var8;
	uint16_t var16;
	uint8_t  var8_3[3];
	uint32_t var32;
} nvram_data_t;


//! NVRAM data structure located in the flash array.
__attribute__((__section__(".flash_nvram")))
static nvram_data_t flash_nvram_data
;

//! NVRAM data structure located in the User page.

__attribute__((__section__(".userpage")))
static nvram_data_t user_nvram_data
;


/*! \brief Prints the variables stored in NVRAM.
 *
 * \param nvram_data  Pointer to the NVRAM data structure to print.
 */
static void print_nvram_variables(nvram_data_t *nvram_data)
{
	print_dbg("var8:\t0x");
	print_dbg_char_hex(nvram_data->var8);

	print_dbg("\r\nvar16:\t0x");
	print_dbg_short_hex(nvram_data->var16);

	print_dbg("\r\nvar8_3:\t0x");
	print_dbg_char_hex(nvram_data->var8_3[0]);
	print_dbg_char_hex(nvram_data->var8_3[1]);
	print_dbg_char_hex(nvram_data->var8_3[2]);

	print_dbg("\r\nvar32:\t0x");
	print_dbg_hex(nvram_data->var32);

	print_dbg("\r\n");
}


/*! \brief This is an example demonstrating flash read / write data accesses
 *         using the FLASHC driver.
 *
 * \param caption     Caption to print before running the example.
 * \param nvram_data  Pointer to the NVRAM data structure to use in the example.
 */
static void flash_rw_example(const char *caption, nvram_data_t *nvram_data)
{
	static const uint8_t write_data[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

	print_dbg(caption);

	print_dbg("Initial values of NVRAM variables:\r\n");
	print_nvram_variables(nvram_data);

	print_dbg("\r\nClearing NVRAM variables...");
	flashc_memset((void *)nvram_data, 0x00, 8, sizeof(*nvram_data), true);
	print_dbg("\r\nNVRAM variables cleared:\r\n");
	print_nvram_variables(nvram_data);

	print_dbg("\r\nWriting new values to NVRAM variables...");
	flashc_memcpy((void *)&nvram_data->var8,   &write_data, sizeof(nvram_data->var8),   true);
	flashc_memcpy((void *)&nvram_data->var16,  &write_data, sizeof(nvram_data->var16),  true);
	flashc_memcpy((void *)&nvram_data->var8_3, &write_data, sizeof(nvram_data->var8_3), true);
	flashc_memcpy((void *)&nvram_data->var32,  &write_data, sizeof(nvram_data->var32),  true);
	print_dbg("\r\nNVRAM variables written:\r\n");
	print_nvram_variables(nvram_data);
}


/*! \brief Main function running the example on both the flash array and the
 *         User page.
 */
int main(void)
{
	// Switch main clock to external oscillator 0 (crystal).
	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

	// Initialize the debug USART module.
	init_dbg_rs232(FOSC0);

	// Apply the example to the flash array.
	flash_rw_example("\x0C=== Using a piece of the flash array as NVRAM ===\r\n",
			&flash_nvram_data);

	// Apply the example to the User page.
	flash_rw_example("\r\n\r\n=== Using a piece of the User page as NVRAM ===\r\n",
			&user_nvram_data);

	while (true);
}
