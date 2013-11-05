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
#include "udc.h"
#include "udi_hid_kbd.h"
#include "board.h"
#include "led.h"
#include "ui.h"

#include "conf_board.h"
#include "pio.h"
#include "pio_handler.h"
#include "sysclk.h"

//! Sequence process running each \c SEQUENCE_PERIOD ms
#define SEQUENCE_PERIOD 150

static struct {
	bool b_modifier;
	bool b_down;
	uint8_t value;
} ui_sequence[] = {
	// Display windows menu
	{true,true,HID_MODIFIER_LEFT_UI},
	// Launch Windows Command line
	{false,true,HID_R},
	{false,false,HID_R},
	// Clear modifier
	{true,false,HID_MODIFIER_LEFT_UI},
	// Tape sequence "notepad" + return
	{false,true,HID_N},
	{false,false,HID_N},
	{false,true,HID_O},
	{false,false,HID_O},
	{false,true,HID_T},
	{false,false,HID_T},
	{false,true,HID_E},
	{false,false,HID_E},
	{false,true,HID_P},
	{false,false,HID_P},
	{false,true,HID_A},
	{false,false,HID_A},
	{false,true,HID_D},
	{false,false,HID_D},
	{false,true,HID_ENTER},
	{false,false,HID_ENTER},
	// Delay to wait "notepad" focus
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	{false,false,0}, // No key (= SEQUENCE_PERIOD delay)
	// Display "Atmel "
	{true,true,HID_MODIFIER_RIGHT_SHIFT}, // Enable Maj
	{false,true,HID_A},
	{false,false,HID_A},
	{true,false,HID_MODIFIER_RIGHT_SHIFT}, // Disable Maj
	{false,true,HID_T},
	{false,false,HID_T},
	{false,true,HID_M},
	{false,false,HID_M},
	{false,true,HID_E},
	{false,false,HID_E},
	{false,true,HID_L},
	{false,false,HID_L},
	{false,true,HID_SPACEBAR},
	{false,false,HID_SPACEBAR},
	// Display "ARM "
	{false,true,HID_CAPS_LOCK}, // Enable caps lock
	{false,false,HID_CAPS_LOCK},
	{false,true,HID_A},
	{false,false,HID_A},
	{false,true,HID_R},
	{false,false,HID_R},
	{false,true,HID_M},
	{false,false,HID_M},
	{false,true,HID_CAPS_LOCK}, // Disable caps lock
	{false,false,HID_CAPS_LOCK},
};

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
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
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
	bool b_btn_state, success;
	static bool btn_last_state = false;
	static bool sequence_running = false;
	static uint8_t sequence_pos = 0;
	uint8_t value;
	static uint16_t cpt_sof = 0;

	if ((framenumber % 1000) == 0) {
		LED_On(LED0_GPIO);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED0_GPIO);
	}
	// Scan process running each 2ms
	cpt_sof++;
	if ((cpt_sof % 2) == 0) {
		return;
	}

	// Scan buttons on switch 0 to send keys sequence
	b_btn_state = (!gpio_pin_is_high(GPIO_PUSH_BUTTON_1)) ? true : false;
	if (b_btn_state != btn_last_state) {
		btn_last_state = b_btn_state;
		sequence_running = true;
	}

	// Sequence process running each period
	if (SEQUENCE_PERIOD > cpt_sof) {
		return;
	}
	cpt_sof = 0;

	if (sequence_running) {
		// Send next key
		value = ui_sequence[sequence_pos].value;
		if (value!=0) {
			if (ui_sequence[sequence_pos].b_modifier) {
				if (ui_sequence[sequence_pos].b_down) {
					success = udi_hid_kbd_modifier_down(value);
				} else {
					success = udi_hid_kbd_modifier_up(value);
				}
			} else {
				if (ui_sequence[sequence_pos].b_down) {
					success = udi_hid_kbd_down(value);
				} else {
					success = udi_hid_kbd_up(value);
				}
			}
			if (!success) {
				return; // Retry it on next schedule
			}
		}
		// Valid sequence position
		sequence_pos++;
		if (sequence_pos >=
			sizeof(ui_sequence) / sizeof(ui_sequence[0])) {
			sequence_pos = 0;
			sequence_running = false;
		}
	}
}

void ui_kbd_led(uint8_t value)
{
	if (value & HID_LED_CAPS_LOCK) {
		LED_On(LED1_GPIO);
	} else {
		LED_Off(LED1_GPIO);
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM3X-EK :
 * - Led 0 (D2, blue) blinks when USB Host have checked and enabled HID Keyboard interface
 * - Led 1 (D3, green) displays caps lock status.
 * - The LEFT CLICK (BP5) opens a notepad application on Windows O.S.
 *   and sends key sequence "Atmel ARM"
 * - RIGHT CLICK (BP4) can be used to wakeup USB Host in remote wakeup mode.
 *
 */
