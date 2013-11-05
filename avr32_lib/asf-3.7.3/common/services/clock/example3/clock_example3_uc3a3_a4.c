/**
 * \file
 *
 * \brief Clock system example 3
 *
 * Copyright (c) 2010 - 2011 Atmel Corporation. All rights reserved.
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
 * This example shows how to switch between various system clock sources
 * and prescalers at run time.
 *
 * \section files Main files:
 * - clock_example3_uc3a3_a4.c: clock system example application
 * - conf_board.h: board initialization configuration
 * - conf_clock.h: system clock configuration
 * - conf_example3.h: chip- or board-specific example configuration
 *
 * \section deviceinfo Device Info
 * All AVR UC3A3 and UC3A4 devices can be used.
 *
 * \section exampledescription Description of the example
 * The clock system is reconfigured every time the user presses a button
 * (\ref NEXT_BUTTON) to use the next clock source and prescaler settings
 * shown in the state diagram below.
 *
 * \par Sequence of clock source and prescaler settings
 * \dot
	digraph clock_example {
		node [shape = doublecircle];
		OSC0 [label = "OSC0"];
		node [shape = ellipse];
		OSC0_DIV [label = "OSC0 / 2"];
		RCOSC [label = "RCOSC"];
		PLL0 [label = "PLL0 / 4"];

		OSC0 -> OSC0_DIV [label="Push button"];
		OSC0_DIV -> RCOSC [label="Push button"];
		RCOSC -> PLL0 [label="Push button"];
		PLL0 -> OSC0 [label="Push button"];
	}
 * \enddot
 *
 * A Timer/Counter (TC) is used to generate periodic interrupts based on
 * its peripheral bus clock. With each interrupt, a LED (\ref LED_EXAMPLE_OUTPUT) is
 * toggled, thus giving a visual indication of the system clock frequency.
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
 * \def NEXT_BUTTON
 * \brief Pin to use for button input
 *
 * This is the active low pin to use for user input.
 *
 * \note The pin is assumed to be configured by the board initialization.
 */
//@}

#include <compiler.h>
#include <gpio.h>
#include <pll.h>
#include <sysclk.h>
#include <delay.h>
#include <conf_example3.h>

ISR( timer_irq_handler, AVR32_TC0_IRQ_GROUP, 3)
{
	static bool toggle;

	/* Slow down the LED a little bit */
	if (toggle)
		gpio_toggle_pin(LED_EXAMPLE_OUTPUT);
	toggle = !toggle;
	AVR32_TC0.channel[0].sr;
}

static void wait_for_switches(void)
{
	do { } while (gpio_pin_is_high(NEXT_BUTTON));
	delay_ms(1);
	do { } while (gpio_pin_is_low(NEXT_BUTTON));
	delay_ms(1);
}

int main(void)
{
	sysclk_init();
	board_init();

	// Initialize the intc sw module.
	irq_initialize_vectors();

	// Register the TC interrupt handler
	irq_register_handler(timer_irq_handler, AVR32_TC0_IRQ0, 3);

	sysclk_enable_pba_module(SYSCLK_TC0);
	AVR32_TC0.channel[0].cmr = AVR32_TC_TCCLKS_TIMER_CLOCK3
		| AVR32_TC_WAVE_MASK;
	AVR32_TC0.channel[0].ier = AVR32_TC_COVFS_MASK;
	AVR32_TC0.channel[0].ccr = AVR32_TC_CLKEN_MASK | AVR32_TC_SWTRG_MASK;

	Enable_global_interrupt();

	while (1) {
		struct pll_config pllcfg;

		/*
		 * Initial state: Running from OSC0 with all prescalers
		 * disabled.
		 */
		wait_for_switches();

		/*
		 * Divide PBA frequency by 2. This will make the LED
		 * blink half as fast.
		 */
		sysclk_set_prescalers(0, 1, 0);
		wait_for_switches();

		/*
		 * Switch to the slow clock with all prescalers
		 * disabled.
		 */
		sysclk_set_source(SYSCLK_SRC_RCSYS);
		sysclk_set_prescalers(0, 0, 0);
		wait_for_switches();

		/*
		 * Enable the PLL at 132 MHz and use it as system clock
		 * with CPU clock at half the system clock frequency and
		 * PBA clock at 1/4 the system clock frequency.
		 */
		pll_config_init(&pllcfg, PLL_SRC_OSC0, 1,
				132000000 / BOARD_OSC0_HZ);
		pll_enable(&pllcfg, 0);
		pll_wait_for_lock(0);
		sysclk_set_prescalers(1, 2, 1);
		sysclk_set_source(SYSCLK_SRC_PLL0);
		wait_for_switches();

		/*
		 * Go back to the initial state and start over.
		 */
		sysclk_set_source(SYSCLK_SRC_OSC0);
		pll_disable(0);
		sysclk_set_prescalers(0, 0, 0);
	}
}
