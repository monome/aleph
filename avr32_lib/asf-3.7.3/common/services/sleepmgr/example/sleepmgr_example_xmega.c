/**
 * \file
 *
 * \brief Sleep manager example for AVR XMEGA
 *
 * Copyright (c) 2010 - 2012 Atmel Corporation. All rights reserved.
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
#include <board.h>
#include <compiler.h>
#include <sleepmgr.h>
#include <ioport.h>
#include <sysclk.h>
#include <conf_example.h>

static void mdelay(uint16_t ms)
{
	uint32_t count;

	// Approximate the number of loop iterations needed.
	count = sysclk_get_cpu_hz() / 1000;
	count *= ms;
        count /= 6;

	do {
		asm("");
	} while (--count);
}

int main(void)
{
	// Set the sleep mode to initially lock.
	enum sleepmgr_mode mode = SLEEPMGR_ACTIVE;
	PORT_t *port;

	board_init();
	sysclk_init();

	// Turn on LED to indicate the device is active.
	ioport_set_pin_low(LED_PIN);

	// Configure pin change interrupt for asynch. wake-up on button pin.
	ioport_configure_pin(BUTTON_PIN, IOPORT_DIR_INPUT | IOPORT_PULL_UP |
			IOPORT_FALLING);

	port = ioport_pin_to_port(BUTTON_PIN);
#if XMEGA_E
	port->INTMASK = PIN2_bm;
	port->INTCTRL = PORT_INTLVL_LO_gc;
#else
	port->INT0MASK = PIN2_bm;
	port->INTCTRL = PORT_INT0LVL_LO_gc;
#endif


	// Enable RTC with ULP as clock source.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_RTC);
	CLK.RTCCTRL = CLK_RTCSRC_ULP_gc | CLK_RTCEN_bm;

	// Configure RTC for wakeup at 1.5 second period (at 256x prescaling).
	RTC.PER = 6;
	RTC.INTCTRL = RTC_OVFINTLVL_LO_gc;

	// Wait until RTC is ready before continuing.
	do { } while (RTC.STATUS & RTC_SYNCBUSY_bm);

	// Initialize the sleep manager, lock initial mode.
	sleepmgr_init();
	sleepmgr_lock_mode(mode);

	// Enable low level interrupts for wakeups to occur.
	PMIC.CTRL = PMIC_LOLVLEN_bm;

	do {
		// Delay for 3 seconds to show the device is awake.
		mdelay(3000);

		// Turn off the LED, restart the RTC and go to sleep.
		ioport_set_pin_high(LED_PIN);
		RTC.CNT = 0;
		RTC.CTRL = RTC_PRESCALER_DIV256_gc;
		do { } while (RTC.STATUS & RTC_SYNCBUSY_bm);
		sleepmgr_enter_sleep();

		// Stop the RTC and turn on the LED.
		RTC.CTRL = RTC_PRESCALER_OFF_gc;
		ioport_set_pin_low(LED_PIN);

		// Unlock current mode, then lock the next one.
		sleepmgr_unlock_mode(mode);
		if (++mode < SLEEPMGR_NR_OF_MODES) {
			sleepmgr_lock_mode(mode);
		} else {
			mode = SLEEPMGR_ACTIVE;
			sleepmgr_lock_mode(mode);
		}
	} while (1);
}

// Interrupt Service Routine definitions
ISR(RTC_OVF_vect)
{
	/* Intentionally left empty. */
}

ISR(BUTTON_vect)
{
	// Wait for button release before continuing.
	mdelay(10);
	do { } while(ioport_pin_is_low(BUTTON_PIN));
	mdelay(10);
}

#if 0
// Example usage in a peripheral ISR/driver.
ISR(DUMMY_vect)
{
	switch (dummy_status) {
	BEGIN:
		sleepmgr_lock(SLEEPMGR_IDLE);
		...
		break;

	...

	DONE:
		...
		sleepmgr_unlock(SLEEPMGR_IDLE);
		break;
	}
}
#endif

/**
 * \mainpage
 *
 * \section intro Introduction
 * This simple example shows how to use the \ref sleepmgr_group.
 *
 * \section files Main files:
 * - sleepmgr_example_xmega.c: Sleep manager example application
 * - conf_example.h: configuration of I/O pins
 *
 * \section apiinfo Sleep manager API
 * The sleep manager API can be found \ref sleepmgr_group "here".
 *
 * \section deviceinfo Device Info
 * All AVR XMEGA devices with an RTC and two free I/O pins can be used.
 * The example has been tested on the Xplain board.
 *
 * \section exampledescription Description of the example
 * This example puts the device to sleep in sleep modes with increasing "depth"
 * and utilizes the RTC and a pin change interrupt to wake it up again.
 *
 * The device will remain in ACTIVE mode for approximately 3 seconds after wake-
 * up, before it goes to sleep in the next mode.
 *
 * The RTC overflows after 1.5 seconds, and will then wake the device up. If
 * the device remains in sleep for longer (LED turned off), the device has been
 * put in a too deep sleep mode for the RTC to operate. When this occurs, the
 * user must wake the device up by means of the pin change interrupt
 * (\ref BUTTON_PIN).
 *
 * A pin (\ref LED_PIN) with, e.g., an on-board LED connected, is driven low to
 * indicate when the device is active. The pin is driven high while the device
 * is in sleep.
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
 * \def BUTTON_PIN
 * \brief ID of pin for asynchronous wake-up of the device
 *
 * This must be pin 2 of a port, for asynchronous wake-up to function.
 * The pin is configured as input, with pull-up and interrupt sensing on falling
 * edges.
 */
/**
 * \def BUTTON_vect
 * \brief Vector of pin change interrupt 0 for \ref BUTTON_PIN
 */
/**
 * \def LED_PIN
 * \brief ID of pin for indication of activity
 *
 * This pin is driven low while the device is active, and high right before the
 * device enters sleep. It may, e.g., be connected to a LED.
 */
//@}
