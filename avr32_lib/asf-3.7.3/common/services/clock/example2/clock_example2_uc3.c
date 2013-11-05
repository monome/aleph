/**
 * \file
 *
 * \brief Clock system example 2
 *
 * Copyright (c) 2010-2013 Atmel Corporation. All rights reserved.
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
 * This example shows how to initialize the clock system and output the
 * CPU clock on a dedicated pin.
 *
 * \section files Main files:
 * - clock_example2_uc3.c: clock system example application
 * - conf_board.h: board initialization configuration
 * - conf_clock.h: system clock configuration
 * - conf_example2.h: chip- or board-specific example configuration
 *
 * \section deviceinfo Device Info
 * All AVR UC3 devices can be used.
 *
 * \section exampledescription Description of the example
 * This example makes use of the output GPIO function for a generic clock to
 * output the CPU clock on a pin.
 *
 * The generic clock and its corresponding GPIO pin and function are defined in
 * conf_example2.h.
 * The main system clock source and prescalers, along with any PLL
 * and/or DFLL configuration, if supported, are defined in conf_clock.h.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

//! \name Example configuration
//@{
/**
 * \def GCLK_ID
 * \brief Generic clock ID
 *
 * This is the ID of the generic clock to use.
 */
/**
 * \def GCLK_PIN
 * \brief Generic clock output GPIO pin
 *
 * This is the GPIO pin on which the generic clock signal will be output.
 *
 * \note Not all GPIO pins can be used for this purpose. Refer to the device's
 * datasheet or device header file for details on which GPIO pins can be used
 * for output of the individual generic clocks.
 */
/**
 * \def GCLK_FUNCTION
 * \brief Generic clock function for GPIO pin
 *
 * This is the GPIO function for output of the generic clock with the configured
 * ID on the GPIO pin.
 */
/**
 * \def GCLK_SOURCE
 * \brief Generic clock source
 *
 * This is the clock source to use for the generic clock.
 */
/**
 * \def GCLK_DIV
 * \brief Generic clock divider
 *
 * This is how much the generic clock should divide the frequency of the
 * configured clock source by.
 */
//@}

#include <compiler.h>
#include <genclk.h>
#include <gpio.h>
#include <sysclk.h>
#include <conf_example2.h>

int main(void)
{

	sysclk_init();
	board_init();

	genclk_enable_config(GCLK_ID, GCLK_SOURCE, GCLK_DIV);

	/* Enable GPIO Alternate to output GCLK*/
	gpio_enable_module_pin(GCLK_PIN,GCLK_FUNCTION);

	while (1) {
		/* Do nothing */
	}
}

