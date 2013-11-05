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
 * - clock_example3_uc3c.c: clock system example application
 * - conf_board.h: board initialization configuration
 * - conf_clock.h: system clock configuration
 * - conf_example3.h: chip- or board-specific example configuration
 *
 * \section deviceinfo Device Info
 * All AVR UC3C devices can be used.
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
		RC8M [label = "RC8M"];
		node [shape = ellipse];
		RC8M_DIV [label = "RC8M / 2"];
		RCSYS;
		OSC0;
		PLL0 [label = "PLL0 / 2"];
		OSC1;
		PLL1 [label = "PLL1 / 2"];

		RC8M -> RC8M_DIV;
		RC8M_DIV -> RCSYS;
		RCSYS -> OSC0 [label = "OSC0 on board"];
		RCSYS -> PLL0 [label = "OSC0 not on board"];
		OSC0 -> PLL0;
		PLL0 -> OSC1 [label = "OSC1 on board"];
		PLL0 -> PLL1 [label = "OSC1 not on board"];
		OSC1 -> PLL1;
		PLL1 -> RC8M;
	}
 * \enddot
 *
 * The Asynchronous Timer (AST) is used to generate periodic interrupts based on
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

ISR( timer_irq_handler, AVR32_AST_IRQ_GROUP, 3)
{
	AVR32_AST.scr = AVR32_AST_PER0_MASK;
	gpio_toggle_pin(LED_EXAMPLE_OUTPUT);
	while (AVR32_AST.sr & AVR32_AST_BUSY_MASK);
}

static void wait_for_switches(void)
{
	do { } while (gpio_pin_is_high(NEXT_BUTTON));
	delay_ms(1);
	do { } while (gpio_pin_is_low(NEXT_BUTTON));
	delay_ms(1);
}

static void ast_init(void)
{
	uint32_t clock;

	sysclk_enable_pba_module(SYSCLK_AST);
	clock = 2 << AVR32_AST_CSSEL;
	AVR32_AST.clock = clock;
	clock |= AVR32_AST_CEN_MASK;
	while (AVR32_AST.sr & AVR32_AST_CLKBUSY_MASK) {
		/* Do nothing */
	}
	AVR32_AST.clock = clock;
	while (AVR32_AST.sr & AVR32_AST_CLKBUSY_MASK) {
		/* Do nothing */
	}

	AVR32_AST.cr = AVR32_AST_EN_MASK;
	while (AVR32_AST.sr & AVR32_AST_BUSY_MASK);
	AVR32_AST.pir0 = 18;
	while (AVR32_AST.sr & AVR32_AST_BUSY_MASK);
	AVR32_AST.scr = AVR32_AST_PER0_MASK;
	while (AVR32_AST.sr & AVR32_AST_BUSY_MASK);
	AVR32_AST.ier = AVR32_AST_PER0_MASK;
}

int main(void)
{
	/* Enable one wait state for flash access */
	AVR32_FLASHC.fcr = AVR32_FLASHC_FWS_MASK;

	sysclk_init();
	board_init();

	// Initialize the intc sw module.
	irq_initialize_vectors();

	// Register the TC interrupt handler
	irq_register_handler(timer_irq_handler, AVR32_AST_PER_IRQ, 3);

	ast_init();
	Enable_global_interrupt();

	while (1) {
		struct pll_config pcfg;

		/*
		 * Initial state: Running from 8 MHz internal RC
		 * oscillator with all prescalers disabled.
		 */
		wait_for_switches();

		/*
		 * Divide PBA frequency by 2. This will make the LED
		 * blink half as fast.
		 */
		sysclk_set_prescalers(0, 1, 0, 0);
		wait_for_switches();

		/*
		 * Switch to the slow clock with all prescalers
		 * disabled.
		 */
		sysclk_set_source(SYSCLK_SRC_RCSYS);
		sysclk_set_prescalers(0, 0, 0, 0);
		osc_disable(OSC_ID_RC8M);
		wait_for_switches();

		/*
		 * Switch to OSC0, if available on the board
		 */
#ifdef BOARD_OSC0_HZ
		osc_enable(OSC_ID_OSC0);
		osc_wait_ready(OSC_ID_OSC0);
		sysclk_set_source(SYSCLK_SRC_OSC0);
		wait_for_switches();
#endif

		/*
		 * Switch to PLL0 running at 80 MHz. Use OSC0 as the
		 * source, if available on board, or RC8M otherwise.
		 */
#ifdef BOARD_OSC0_HZ
		pll_config_init(&pcfg, PLL_SRC_OSC0, 1,
				80000000 / BOARD_OSC0_HZ);
#else
		osc_enable(OSC_ID_RC8M);
		pll_config_init(&pcfg, PLL_SRC_RC8M, 1,
				80000000 / OSC_RC8M_NOMINAL_HZ);
#endif
		pll_enable(&pcfg, 0);
		sysclk_set_prescalers(1, 1, 1, 1);
		pll_wait_for_lock(0);
		sysclk_set_source(SYSCLK_SRC_PLL0);
		wait_for_switches();

		/*
		 * Switch to OSC1, if available on the board
		 */
#ifdef BOARD_OSC1_HZ
		osc_enable(OSC_ID_OSC1);
		osc_wait_ready(OSC_ID_OSC1);
		sysclk_set_source(SYSCLK_SRC_OSC1);
#ifndef BOARD_OSC0_HZ
		osc_disable(OSC_ID_RC8M);
#endif
		wait_for_switches();
#endif

		/*
		 * Switch to PLL1 running at 64 MHz. Use OSC1 as the
		 * source, if available on board, or RC8M otherwise.
		 */
#ifdef BOARD_OSC1_HZ
		pll_config_init(&pcfg, PLL_SRC_OSC1, 1,
				64000000 / BOARD_OSC1_HZ);
#else
# ifdef BOARD_OSC0_HZ
		osc_enable(OSC_ID_RC8M);
# endif
		pll_config_init(&pcfg, PLL_SRC_RC8M, 1,
				64000000 / OSC_RC8M_NOMINAL_HZ);
#endif
		pll_enable(&pcfg, 1);
		sysclk_set_prescalers(1, 1, 1, 1);
		pll_wait_for_lock(1);
		sysclk_set_source(SYSCLK_SRC_PLL1);
		pll_disable(0);
#if defined(BOARD_OSC0_HZ)
		osc_disable(OSC_ID_OSC0);
#elif defined(BOARD_OSC1_HZ)
		osc_disable(OSC_ID_RC8M);
#endif
		wait_for_switches();

		/*
		 * Go back to the initial state and start over.
		 */
#ifdef BOARD_OSC1_HZ
		osc_enable(OSC_ID_RC8M);
#endif
		osc_wait_ready(OSC_ID_RC8M);
		sysclk_set_source(SYSCLK_SRC_RC8M);
		sysclk_set_prescalers(0, 0, 0, 0);
		pll_disable(1);
#ifdef BOARD_OSC1_HZ
		osc_disable(OSC_ID_OSC1);
#endif
	}
}
