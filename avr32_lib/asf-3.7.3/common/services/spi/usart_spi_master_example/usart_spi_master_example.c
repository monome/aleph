/*****************************************************************************
 *
 * \file
 *
 * \brief Example of usage of the USART_SPI Master Mode Basic Services.
 *
 ****************************************************************************/

/**
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
/*! \mainpage
 * \section intro Introduction
 * This example demonstrates how to use the USART_SPI Master Mode Basic
 * Services.
 *
 * \section files Main Files
 * - usart_spi_master_example.c: port drivers example application.
 * - conf_board.h: board initialization process configuration
 * - conf_usart_spi_master_example.h: board-specific mapping of resources used
 *    by this example
 * - usart_spi.c: Part Specific USART_SPI Master Mode Implementation
 *
 * \section usart_spi API
 * The usart_spi API can be found \ref usart_spi.h "here".
 *
 * \section deviceinfo Device Info
 * This example can be used on all SAM EK boards with external AT45DBX data
 * flash component. Connect the board and external component with the
 * following paired pins.
 * \copydoc usart_spi_master_example_pin_defs
 *
 * \section exampledescription Description of the example
 *   - Send "Manufacturer ID Read" command to the dataflash.
 *   - Read back the Manufacturer ID of the dataflash.
 *   - If Manufacturer ID match Atmel ID "0x1F", both
 *      \ref SPI_EXAMPLE_LED_PIN_EXAMPLE_1 and
 *      \ref SPI_EXAMPLE_LED_PIN_EXAMPLE_2 are 'on'. Otherwise
 *      \ref SPI_EXAMPLE_LED_PIN_EXAMPLE_1 is 'on' and
 *      \ref SPI_EXAMPLE_LED_PIN_EXAMPLE_2 is 'off'.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

#include "asf.h"
#include "conf_usart_spi_master_example.h"

/* Manufacturer ID for dataflash. */
uint8_t manufacturer_id;

/* Manufacturer ID for Atmel dataflash. */
#define ATMEL_MANUFACTURER_ID         0x1F

/* AT45DBX Command: Manufacturer ID Read. */
#define AT45DF_CMDC_RD_MID_REG        0x9F

/* Buffer size. */
#define DATA_BUFFER_SIZE         0x04

/* Data buffer. */
uint8_t data[DATA_BUFFER_SIZE] = {AT45DF_CMDC_RD_MID_REG};

struct usart_spi_device USART_SPI_DEVICE_EXAMPLE = {
	/* Board specific select ID. */
	.id = USART_SPI_DEVICE_EXAMPLE_ID
};

static bool usart_spi_at45dbx_mem_check(void)
{
	/* Select the DF memory to check. */
	usart_spi_select_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE);

	/* Send the Manufacturer ID Read command. */
	usart_spi_write_packet(USART_SPI_EXAMPLE, data, 1);

	/* Receive Manufacturer ID. */
	usart_spi_read_packet(USART_SPI_EXAMPLE, data, DATA_BUFFER_SIZE);

	/* Extract the Manufacturer ID. */
	manufacturer_id = data[0];

	/* Deselect the checked DF memory. */
	usart_spi_deselect_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE);

	/* Check the Manufacturer id. */
	if (manufacturer_id == ATMEL_MANUFACTURER_ID) {
		return true;
	} else {
		return false;
	}
}


/*! \brief Main function.
 */
int main(void)
{
	sysclk_init();

	/*
	 * Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();

	/* Config the USART_SPI. */
	usart_spi_init(USART_SPI_EXAMPLE);
	usart_spi_setup_device(USART_SPI_EXAMPLE, &USART_SPI_DEVICE_EXAMPLE,
			SPI_MODE_0, USART_SPI_EXAMPLE_BAUDRATE, 0);
	usart_spi_enable(USART_SPI_EXAMPLE);

	/* Show the test result by LED. */
	if (usart_spi_at45dbx_mem_check() == false) {
		gpio_set_pin_low(USART_SPI_EXAMPLE_LED_PIN_EXAMPLE_1);
		gpio_set_pin_high(USART_SPI_EXAMPLE_LED_PIN_EXAMPLE_2);
	} else {
		gpio_set_pin_low(USART_SPI_EXAMPLE_LED_PIN_EXAMPLE_1);
		gpio_set_pin_low(USART_SPI_EXAMPLE_LED_PIN_EXAMPLE_2);
	}

	while (1) {
		/* Do nothing */
	}
}

