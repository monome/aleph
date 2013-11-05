/**
 * \file
 *
 * \brief User Interface
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

#include "compiler.h"
#include "conf_usb.h"
#include "udi_msc.h"
#include "udc.h"
#include "udi_hid_mouse.h"
#include "board.h"
#include "led.h"
#include "ui.h"
#include "pio.h"
#include "pio_handler.h"
#include "pmc.h"

#define  MOUSE_MOVE_RANGE  3

// Wakeup pin is RIGHT CLICK (fast wakeup 14)
#define  WAKEUP_PMC_FSTT (PMC_FSMR_FSTT14)
#define  WAKEUP_PIN      (GPIO_PUSH_BUTTON_2)
#define  WAKEUP_PIO      (PIN_PUSHBUTTON_2_PIO)
#define  WAKEUP_PIO_ID   (PIN_PUSHBUTTON_2_ID)
#define  WAKEUP_PIO_MASK (PIN_PUSHBUTTON_2_MASK)
#define  WAKEUP_PIO_ATTR (PIN_PUSHBUTTON_2_ATTR)

// Interrupt on "pin change" from RIGHT CLICK to do wakeup on USB
// Note:
// This interrupt is enable when the USB host enable remotewakeup feature
// This interrupt wakeup the CPU if this one is in idle mode
static void ui_wakeup_handler(uint32_t id, uint32_t mask)
{
	if (WAKEUP_PIO_ID == id && WAKEUP_PIO_MASK == mask) {
		// It is a wakeup then send wakeup USB
		udc_remotewakeup();
	}
}

void ui_init(void)
{
	// Enable PIO clock for button inputs
	pmc_enable_periph_clk(ID_PIOC);
	pmc_enable_periph_clk(ID_PIOD);
	// Set handler for wakeup
	pio_handler_set(WAKEUP_PIO, WAKEUP_PIO_ID, WAKEUP_PIO_MASK, WAKEUP_PIO_ATTR, ui_wakeup_handler);
	// Enable IRQ for button (PIOC and PIOD)
	NVIC_EnableIRQ((IRQn_Type) WAKEUP_PIO_ID);
	// Initialize LEDs
	LED_On(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
}


void ui_wakeup_enable(void)
{
	// Enable interrupt for button pin
	pio_configure_pin(WAKEUP_PIN, WAKEUP_PIO_ATTR);
	pio_enable_pin_interrupt(WAKEUP_PIN);
	// Enable fast wakeup for button pin
	pmc_set_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup_disable(void)
{
	// Disable interrupt for button pin
	pio_disable_pin_interrupt(WAKEUP_PIN);
	// Disable fast wakeup for button pin
	pmc_clr_fast_startup_input(WAKEUP_PMC_FSTT);
}

void ui_wakeup(void)
{
	LED_On(LED0_GPIO);
}

void ui_process(uint16_t framenumber)
{
	static uint8_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED0_GPIO);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED0_GPIO);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if (cpt_sof < 2) {
		return;
	}
	cpt_sof = 0;

	// Uses buttons to move mouse
	if (!gpio_pin_is_high(GPIO_PUSH_BUTTON_1))
		udi_hid_mouse_moveX(-MOUSE_MOVE_RANGE);
	if (!gpio_pin_is_high(GPIO_PUSH_BUTTON_2))
		udi_hid_mouse_moveX( MOUSE_MOVE_RANGE);
}

void ui_start_read(void)
{
	LED_On(LED1_GPIO);
}

void ui_stop_read(void)
{
	LED_Off(LED1_GPIO);
}

void ui_start_write(void)
{
	LED_On(LED2_GPIO);
}

void ui_stop_write(void)
{
	LED_Off(LED2_GPIO);
}


/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM3X-EK:
 * - Led 0 (D2, blue) blinks when USB Host have checked and enabled HID and MSC interface
 * - Led 1 (D3, green) is on during read operation
 * - Led 2 (D4, amber) is on during write operation
 * - LEFT CLICK (BP5) and RIGHT CLICK (BP4) are used to move mouse left and right
 * - RIGHT CLICK (BP4) can be used to wakeup USB Host in remote wakeup mode.
 *
 */
