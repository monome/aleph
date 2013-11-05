/**
 * \file
 *
 * \brief Sleep manager example for SAM4L series
 *
 * Copyright (C) 2012 Atmel Corporation. All rights reserved.
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

#include <asf.h>
#include <conf_example.h>
#include "ast.h"

void AST_PER_Handler(void)
{
	ast_clear_periodic_status_flag(AST, 0);
}

int main(void)
{
	enum sleepmgr_mode current_sleep_mode = SLEEPMGR_ACTIVE;
	uint32_t ast_counter = 0;

	/*
	 * Initialize the synchronous clock system to the default configuration
	 * set in conf_clock.h.
	 * \note All non-essential peripheral clocks are initially disabled.
	 */
	sysclk_init();

	/*
	 * Initialize the resources used by this example to the default
	 * configuration set in conf_board.h
	 */
	board_init();

	/*
	 * Turn the activity status LED on to inform the user that the device
	 * is active.
	 */
	ioport_set_pin_level(LED_ACTIVITY_STATUS_PIN, LED_STATUS_ON);

	osc_priv_enable_osc32();

	/* Enable the AST clock. */
	sysclk_enable_pba_module(SYSCLK_AST);
	/* Initialize the AST in Counter mode. */
	ast_init_counter(AST, AST_OSC_1KHZ, AST_PSEL_32KHZ_1HZ - 6,
			ast_counter);

	/*
	 * Configure the AST to wake up the CPU when the counter reaches the
	 * selected periodic0 value.
	 */
	ast_set_periodic0_value(AST,AST_PSEL_32KHZ_1HZ - 3);
	ast_enable_periodic_interrupt(AST,0);
	ast_enable_periodic_async_wakeup(AST,0);
	ast_enable_periodic0(AST);
	ast_clear_periodic_status_flag(AST,0);

	NVIC_ClearPendingIRQ(AST_PER_IRQn);
	NVIC_EnableIRQ(AST_PER_IRQn);

	/* Enable the AST. */
	ast_enable(AST);

	/* AST can wakeup the device */
	bpm_enable_wakeup_source(BPM, (1 << BPM_BKUPWEN_AST));

	// Initialize the sleep manager, lock initial mode.
	sleepmgr_init();
	sleepmgr_lock_mode(current_sleep_mode);

	while (1) {
		/*
		 * Turn the activity status LED off to inform the user that the
		 * device is in a sleep mode.
		 */
		ioport_set_pin_level(LED_ACTIVITY_STATUS_PIN, LED_STATUS_OFF);

		/*
		 * Go to sleep in the deepest allowed sleep mode (i.e. no
		 * deeper than the currently locked sleep mode).
		 */
		sleepmgr_enter_sleep();

		/*
		 * Turn the activity status LED on to inform the user that the
		 * device is active.
		 */
		ioport_set_pin_level(LED_ACTIVITY_STATUS_PIN, LED_STATUS_ON);

		/* Unlock the current sleep mode. */
		sleepmgr_unlock_mode(current_sleep_mode);

		/* Add a 3s delay. */
		delay_s(3);

		/* Lock the next sleep mode. */
		++current_sleep_mode;
		if ((current_sleep_mode >= SLEEPMGR_NR_OF_MODES)) {
			current_sleep_mode = SLEEPMGR_ACTIVE;
		}

		sleepmgr_lock_mode(current_sleep_mode);
	}
}

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref sleepmgr_group.
 *
 * \section files Main files:
 * - sleepmgr_example_sam4l_ast.c: Sleep manager example application
 * - conf_example.h: configuration of I/O pins
 *
 * \section apiinfo Sleep manager API
 * The sleep manager API can be found \ref sleepmgr_group "here".
 *
 * \section deviceinfo Device Info
 * All SAM4 L series devices can be used.
 * The example has been tested on the SAM4L-EK boards.
 *
 * \section exampledescription Description of the example
 * The device is put to sleep in sleep modes with increasing "depth", and is
 * periodically woken up by the AST or asynchronously by an external interrupt
 * for the modes which also disable the RTC.
 * The device will remain in ACTIVE mode for approximately 3 seconds after wake-
 * up, before it goes to sleep in the next mode.
 *
 * The AST wakes the CPU every 3.5 seconds. If the device remains in sleep (LED
 * turned off), the device has gone into a too deep sleep mode for the AST to
 * operate (in the case of this example, the chosen source clock for the AST is
 * the RCOSC and the RCOSC is stopped in the WAIT/RET sleep mode). In that case,
 * the user must wake the device up by applying a low-level on the external
 * interrupt pin, e.g., by pushing a button connected to the pin
 * (\ref WAKE_BUTTON_EIC_PIN).
 *
 * A pin (\ref LED_ACTIVITY_STATUS_PIN) with, e.g., a LED connected is driven
 * low when the device is active and driven high when the device is sleeping.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 * Support and FAQ: http://support.atmel.no/
 */

//! \name Example configuration
//@{
/**
 * \def WAKE_BUTTON_EIC_PIN
 * \brief The active-low GPIO pin to exit the WAIT/RET mode (by generating
 * an external interrupt).
 */
/**
 * \def WAKE_BUTTON_EIC_FUNCTION
 * \brief The EIC function of the wake pin.
 */
/**
 * \def WAKE_BUTTON_EIC_LINE
 * \brief The EIC line of the wake pin.
 */
/**
 * \def LED_ACTIVITY_STATUS_PIN
 * \brief The LED that will be turned off when the device is in a sleep mode
 * and on when it is active.
 */
//@}
