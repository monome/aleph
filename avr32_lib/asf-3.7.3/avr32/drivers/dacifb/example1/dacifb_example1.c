/*****************************************************************************
*
* \file
*
* \brief DACIFB example driver for AVR32 UC3.
*
* Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
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
*****************************************************************************/

/** \mainpage
 * \section intro Introduction
 * This is the documentation for the data structures, functions, variables,
 * defines, enums, and typedefs for the DACIFB driver. It also comes
 * bundled with an application-example of usage.
 *
 * This example demonstrates how to use the DACIFB driver with the internal
 * timer as sampling rate source. The DACIFB collects data through the PDCA.
 * Once all the data are transmitted to the DACIFB, a PDCA transfer is reloaded.
 * <b>Operating mode: </b> On DAC output [PA15 (microphone output) on UC3C_EK] a
 * sinewave is generated. All the element of the sinewave, stored in an
 * internal buffer, are transmitted to the PDCA through the DACIFB.
 *
 * \section files Main Files
 * - sinus.c: Sinusoid sound sample array.
 * - sinus.h: Header file for the Sinusoid sound sample array.
 * - conf_example.h: Example board configuration constants
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC for AVR32 and for IAR Embedded Workbench
 * for Atmel AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All AVR32 devices with an DACIFB and a PDCA module can be used.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - UC3C_EK evaluation kit;
 * - CPU clock: 16 MHz;
 *
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */

#include <asf.h>
#include "conf_example.h"
#include "sinus.h"

/* Assign the on-board sensors to their DAC channel. */
int16_t dac_value_audio   = -1;
uint8_t dac_channel_audio = EXAMPLE_DAC_AUDIO_CHANNEL;

/* DAC Configuration */
dacifb_opt_t dacifb_opt = {
	.reference            = DACIFB_REFERENCE_VDDANA,
	.channel_selection    = DACIFB_CHANNEL_SELECTION_B,
	.low_power            = false,
	.dual                 = false,
	.prescaler_clock_hz   = EXAMPLE_DAC_PRESCALER_CLOCK
};

/* DAC Channel Configuration */
dacifb_channel_opt_t dacifb_channel_opt = {
	.auto_refresh_mode    = false,
	.trigger_mode         = DACIFB_TRIGGER_MODE_TIMER,
	.left_adjustment      = false,
	.data_shift           = 0,
	.data_round_enable    = false
};

/* PDCA channel options */
pdca_channel_options_t pdca_opt = {
	.addr                 = (void *)sin_table, 
	.pid                  = AVR32_PDCA_PID_DAC_TX,
	.size                 = SINUS_SAMPLES,
	.r_addr               = NULL,
	.r_size               = 0,
	.transfer_size        = PDCA_TRANSFER_SIZE_HALF_WORD
};

/** PDCA interrupt handler, fired each time the complete sine wave table
 *  has been sent to the DAC. */
#if (UC3C)
ISR(pdca_int_handler, AVR32_PDCA_IRQ_GROUP0, 0)
#else
ISR(pdca_int_handler, AVR32_PDCA_IRQ_GROUP, 0)
#endif
{
	/* Set PDCA channel reload values with the address of the new data to
	 * send is stored, and size of the data block to load. */
	pdca_reload_channel(EXAMPLE_PDCA_CHANNEL_DAC, (void *)sin_table,
			SINUS_SAMPLES);
}

/** Initialize interrupt controller and register PDCA interrupt handler. */
static void pdca_set_irq(void)
{
#if defined (__GNUC__)
	/* Disable all interrupt/exception. */
	Disable_global_interrupt();

	INTC_init_interrupts();

	INTC_register_interrupt((__int_handler) & pdca_int_handler,
			AVR32_PDCA_IRQ_0, AVR32_INTC_INT0);
#endif

	/* Enable all interrupt/exception. */
	Enable_global_interrupt();
}

/** Main function to configure the DAC and DMA, and begin conversions. */
int main(void)
{
	/* GPIO pin/DAC-function map. */
	const gpio_map_t DACIFB_GPIO_MAP = {
		{AVR32_DACREF_PIN, AVR32_DACREF_FUNCTION},
		{AVR32_ADCREFP_PIN, AVR32_ADCREFP_FUNCTION},
		{AVR32_ADCREFN_PIN, AVR32_ADCREFN_FUNCTION},
		{EXAMPLE_DAC_AUDIO_PIN, EXAMPLE_DAC_AUDIO_FUNCTION}
	};

	/* Initialize the system clocks */
	sysclk_init();

	/* Assign and enable GPIO pins to the DAC function. */
	gpio_enable_module(DACIFB_GPIO_MAP, sizeof(DACIFB_GPIO_MAP) /
			sizeof(DACIFB_GPIO_MAP[0]));

	/* Get DACIFB Factory Configuration */
	dacifb_get_calibration_data(&AVR32_DACIFB1,
			&dacifb_opt,
			EXAMPLE_DAC_AUDIO_INSTANCE);

	/* Configure DACIFB */
	dacifb_configure(&AVR32_DACIFB1,
			&dacifb_opt,
			FOSC0);

	/* Configure channel DACIFB */
	dacifb_configure_channel(&AVR32_DACIFB1,
			dac_channel_audio,
			&dacifb_channel_opt,
			EXAMPLE_DAC_PRESCALER_CLOCK);

	dacifb_start_channel(&AVR32_DACIFB1,
			dac_channel_audio,
			FOSC0);

	dacifb_reload_timer(&AVR32_DACIFB1,
			dac_channel_audio,
			8,
			EXAMPLE_DAC_PRESCALER_CLOCK);

	/* Init PDCA channel with the requested options. */
	pdca_init_channel(EXAMPLE_PDCA_CHANNEL_DAC, &pdca_opt);

	/* Register PDCA IRQ interrupt. */
	pdca_set_irq();

	/* Enable pdca interrupt each time the reload counter reaches zero. */
	pdca_enable_interrupt_reload_counter_zero(EXAMPLE_PDCA_CHANNEL_DAC);

	/* Enable now the transfer. */
	pdca_enable(EXAMPLE_PDCA_CHANNEL_DAC);

	for (;;) {
		/* Do nothing; interrupts handle the DAC conversions */
	}
}
