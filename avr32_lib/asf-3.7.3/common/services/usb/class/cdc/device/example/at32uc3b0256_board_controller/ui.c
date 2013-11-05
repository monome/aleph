/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2011 - 2012 Atmel Corporation. All rights reserved.
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

static volatile bool ui_b_dtr_enable = false;
static volatile bool ui_b_error_occur = false;

void ui_init(void)
{
	LED_Off(LED0);
	LED_Off(LED1);
}

void ui_powerdown(void)
{
	LED_Off(LED0);
	LED_Off(LED1);
}

void ui_wakeup(void)
{
}

void ui_com_open(uint8_t port)
{
	// Here the COM port has been open on terminal
	ui_b_dtr_enable = true;
	ui_b_error_occur = false; // Reset error notification
}

void ui_com_close(uint8_t port)
{
	// Here the COM port has been close on terminal
	ui_b_dtr_enable = false;
	ui_b_error_occur = false; // Reset error notification
}

void ui_com_rx_start(void)
{
	LED_On(LED1);
}

void ui_com_rx_stop(void)
{
}

void ui_com_tx_start(void)
{
	LED_On(LED1);
}

void ui_com_tx_stop(void)
{
}

void ui_com_error(void)
{
	ui_b_error_occur = true;
}

void ui_com_overflow(void)
{
	ui_b_error_occur = true;
}

void ui_process(uint16_t framenumber)
{
	static uint16_t sof_counter=0;
	uint16_t frequency;

	if (!ui_b_dtr_enable) {
		// Port not open by terminal
		// Blink each 1s
		frequency = 1000;
	}else{
		if (!ui_b_error_occur) {
			// Port is open by terminal
			// Blink each 0.5s
			frequency = 500;
		}else{
			// Port is open and an error has occurred
			// Blink each 250ms
			frequency = 250;
		}
	}
	if ((sof_counter++ % frequency) == 0) {
		LED_Toggle(LED0);
		LED_Off(LED1);
	}
}


/**
 * \defgroup UI User Interface
 *
 * Human interface on uc3b board controller:
 * - Led 0 blink (each 1s) when CDC is enumerated by host
 * - Led 0 blink (each 0.5s) when communication port is open by a terminal
 * - Led 0 blink (each 0.25s) when an underflow/overflow occurs
 * - Led 1 is on during data transfer
 */
