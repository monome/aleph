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

#include <asf.h>
#include "ui.h"
#include "ieee11073_skeleton.h"

void ui_init(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
}

void ui_wakeup(void)
{
	LED_On(LED0_GPIO);
}

void ui_association(bool state)
{
	if (state) {
		LED_On(LED2_GPIO);
	} else {
		LED_Off(LED2_GPIO);
	}
}

void ui_process(uint16_t framenumber)
{
	static uint8_t cpt_sof = 0;
	bool b_btn_state;
	static bool btn0_last_state = false;
	static bool btn1_last_state = false;

	if ((framenumber % 1000) == 0) {
		LED_On(LED1_GPIO);
	}

	if ((framenumber % 1000) == 500) {
		LED_Off(LED1_GPIO);
	}

	/* Scan process running each 20ms */
	cpt_sof++;
	if (20 > cpt_sof) {
		return;
	}

	cpt_sof = 0;

	/* Scan buttons on switch 0 (left), 1 (middle), 2 (right) */
	b_btn_state = gpio_pin_is_low(GPIO_PUSH_BUTTON_0);
	if (b_btn_state != btn0_last_state) {
		btn0_last_state = b_btn_state;
		if (b_btn_state) {
			ieee11073_skeleton_send_measure_1();
		}
	}

	b_btn_state = gpio_pin_is_low(GPIO_PUSH_BUTTON_1);
	if (b_btn_state != btn1_last_state) {
		btn1_last_state = b_btn_state;
		if (b_btn_state) {
			ieee11073_skeleton_send_measure_2();
		}
	}
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on XMEGA-A3BU Xplained:
 * - Led green (close to USB connector) is on at power on
 * - Led 0 is on when USB line is in IDLE mode, and off in SUSPEND mode
 * - Led 1 blinks when USB host has checked and enabled PHDC interface
 * - Led 2 is on when PHDC has validated association
 * - SW0 and SW1 Push Button can be used to send a measure
 */
