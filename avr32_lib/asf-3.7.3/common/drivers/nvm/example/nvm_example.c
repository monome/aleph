/**
 * \file
 *
 * \brief Common NVM service example
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \mainpage
 *
 * \section intro Introduction
 * This example demonstrates how to use the basic Non volatile memories
 * access functions
 *
 * \section files Main files
 * - nvm_example.c: example application
 * - conf_board.h: board configuration
 * - conf_nvm.h: Non volatile memory configuration used in this example
 * - conf_nvm_example.h: board-specific mapping of resources
 * used by this example
 *
 * \section nvmapiinfo drivers/basic/nvm API
 * The NVM API can be found \ref common_nvm.h "here".
 *
 * \section deviceinfo Device Info
 * All AVR and SAM devices with internal Flash can be used.
 *
 * \section exampledescription Description of the example
 *   - Write data to the internal flash and/or external dataflash AT45DBX
 *   - Read back the data and compare with the original data
 *
 * NVM_EXAMPLE_LED0 gives the result for internal flash:\n
 * NVM_EXAMPLE_LED0 OFF: Test failed
 * NVM_EXAMPLE_LED0 ON : Test passed
 *
 * On boards with external dataflash,
 * NVM_EXAMPLE_LED1 gives the result for external dataflash AT45DBX:\n
 * NVM_EXAMPLE_LED1 OFF: Test failed
 * NVM_EXAMPLE_LED1 ON : Test passed
 *
 * NVM_EXAMPLE_LED2 ON: Indicates that all tests are complete
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

#include "asf.h"
#include "conf_nvm_example.h"

/*! \brief Test routine that writes to the non volatile memory, reads back
 * and compares the values
 */
static status_code_t test_mem(mem_type_t mem, uint32_t test_address)
{
	static uint8_t write_buf[8]
		= {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
	static uint8_t read_buf[8], i = 0;

	/* Initialize the non volatile memory */
	if (nvm_init(mem) != STATUS_OK) {
		return ERR_INVALID_ARG;
	}

	/* Write test pattern to the specified address */
	nvm_write(mem, test_address, (void *)write_buf, sizeof(write_buf));

	/* Read back data from the same address */
	nvm_read(mem, test_address, (void *)read_buf, sizeof(read_buf));

	/* Validate the read data */
	for (i = 0; i < sizeof(write_buf); i++) {
		if (read_buf[i] != write_buf[i]) {
			return ERR_BAD_DATA;
		}
	}

	return STATUS_OK;
}

/*! \brief NVM Example Main
 *
 * The NVM example begins by initializing required board resources.  The
 * system clock and basic GPIO pin mapping are established.
 *
 * A memory location on the Non volatile memory is written with a fixed test
 * pattern which is then read back into a separate buffer. As a basic sanity
 * check, the original write-buffer values are compared with values read to
 * a separate buffer.  An LED on the development board is illuminated when there
 * is a match between the written and read data.
 *
 * \return Nothing.
 */
int main(void)
{
	mem_type_t mem;

	sysclk_init();
	board_init();

	/* Test internal flash */
	mem = INT_FLASH;
	if (test_mem(mem, (uint32_t)TEST_ADDRESS_INT) == STATUS_OK) {
		/* Turn on LED to indicate success */
		ioport_set_pin_level(NVM_EXAMPLE_LED0, false);
	}

#if defined(USE_EXTMEM) && defined(CONF_BOARD_AT45DBX)
	/* Test external dataflash */
	mem = AT45DBX;
	if (test_mem(mem, (uint32_t)TEST_ADDRESS_EXT) == STATUS_OK) {
		/* Turn on LED to indicate success */
		ioport_set_pin_level(NVM_EXAMPLE_LED1, false);
	}
#endif

	/* Turn on LED to indicate tests are complete */
	ioport_set_pin_level(NVM_EXAMPLE_LED2, false);

	while (1) {
	}
}
