/**
 * \file
 *
 * \brief UART functions
 *
 * Copyright (C) 2012 - 2013 Atmel Corporation. All rights reserved.
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

#include "conf_example.h"
#include "conf_usb_host.h"
#include <asf.h>

#if SAM4L
#   define USART_PERIPH_CLK_ENABLE() sysclk_enable_peripheral_clock(USART_BASE)
#else
#   define USART_PERIPH_CLK_ENABLE() sysclk_enable_peripheral_clock(USART_ID)
#endif


/* Default option */
static sam_usart_opt_t usart_options = {
	.baudrate = 115200,
	.char_length = 8,
	.parity_type = US_MR_PAR_NO,
	.stop_bits = US_MR_NBSTOP_1_BIT,
	.channel_mode = US_MR_CHMODE_NORMAL,
};

ISR(USART_HANDLER)
{
	uint32_t sr = usart_get_status(USART_BASE);
	if (sr & US_CSR_RXRDY) {
		/* Data received */
		ui_com_tx_start();
		uint32_t value;
		bool b_error = usart_read(USART_BASE, &value) ||
			(sr & (US_CSR_FRAME | US_CSR_TIMEOUT | US_CSR_PARE));
		if (b_error) {
			usart_reset_rx(USART_BASE);
			usart_enable_rx(USART_BASE);
			ui_com_error();
		}

		/* Transfer UART RX fifo to CDC TX */
		if (!uhi_cdc_is_tx_ready(0)) {
			/* Fifo full */
			ui_com_overflow();
		} else {
			uhi_cdc_putc(0, value);
		}

		ui_com_tx_stop();
		return;
	}

	if (sr & US_CSR_TXRDY) {
		/* Data ready to be sent */
		if (uhi_cdc_is_rx_ready(0)) {
			/* Transmit next data */
			ui_com_rx_start();
			int c = uhi_cdc_getc(0);
			usart_write(USART_BASE, c);
		} else {
			/* Fifo empty then Stop UART transmission */
			usart_disable_tx(USART_BASE);
			usart_disable_interrupt(USART_BASE, US_IDR_TXRDY);
			ui_com_rx_stop();
		}
	}
}

void uart_rx_notify(void)
{
	/* If UART is open */
	if (usart_get_interrupt_mask(USART_BASE)
		& US_IMR_RXRDY) {
		/* Enable UART TX interrupt to send a new value */
		usart_enable_tx(USART_BASE);
		usart_enable_interrupt(USART_BASE, US_IER_TXRDY);
	}
}

void uart_config(usb_cdc_line_coding_t *cfg)
{
	uint32_t stopbits, parity, databits;
	uint32_t imr;

	switch (cfg->bCharFormat) {
	case CDC_STOP_BITS_2:
		stopbits = US_MR_NBSTOP_2_BIT;
		break;

	case CDC_STOP_BITS_1_5:
		stopbits = US_MR_NBSTOP_1_5_BIT;
		break;

	case CDC_STOP_BITS_1:
	default:
		/* Default stop bit = 1 stop bit */
		stopbits = US_MR_NBSTOP_1_BIT;
		break;
	}

	switch (cfg->bParityType) {
	case CDC_PAR_EVEN:
		parity = US_MR_PAR_EVEN;
		break;

	case CDC_PAR_ODD:
		parity = US_MR_PAR_ODD;
		break;

	case CDC_PAR_MARK:
		parity = US_MR_PAR_MARK;
		break;

	case CDC_PAR_SPACE:
		parity = US_MR_PAR_SPACE;
		break;

	default:
	case CDC_PAR_NONE:
		parity = US_MR_PAR_NO;
		break;
	}

	switch(cfg->bDataBits) {
	case 5: case 6: case 7:
		databits = cfg->bDataBits - 5;
		break;
	default:
	case 8:
		databits = US_MR_CHRL_8_BIT;
		break;
	}

	/* Options for USART. */
	usart_options.baudrate = LE32_TO_CPU(cfg->dwDTERate);
	usart_options.char_length = databits;
	usart_options.parity_type = parity;
	usart_options.stop_bits = stopbits;
	usart_options.channel_mode = US_MR_CHMODE_NORMAL;
	imr = usart_get_interrupt_mask(USART_BASE);
	usart_disable_interrupt(USART_BASE, 0xFFFFFFFF);
	usart_init_rs232(USART_BASE, &usart_options,
			sysclk_get_peripheral_bus_hz(USART_BASE));
	/* Restore both RX and TX */
	usart_enable_tx(USART_BASE);
	usart_enable_rx(USART_BASE);
	usart_enable_interrupt(USART_BASE, imr);
}

void uart_open(void)
{
	/* IO is initialized in board init
	 * Enable interrupt with priority higher than USB
	 */
	NVIC_SetPriority(USART_INT_IRQn, USART_INT_LEVEL);
	NVIC_EnableIRQ(USART_INT_IRQn);

	/* Initialize it in RS232 mode. */
	USART_PERIPH_CLK_ENABLE();
	if (usart_init_rs232(USART_BASE, &usart_options,
			sysclk_get_peripheral_bus_hz(USART_BASE))) {
		return;
	}

	/* Enable USART */
	USART_ENABLE();

	/* Enable both RX and TX */
	usart_enable_tx(USART_BASE);
	usart_enable_rx(USART_BASE);
	/* Enable interrupts */
	usart_enable_interrupt(USART_BASE, US_IER_RXRDY | US_IER_TXRDY);
}

void uart_close(void)
{
	/* Disable interrupts */
	usart_disable_interrupt(USART_BASE, 0xFFFFFFFF);
	/* Close RS232 communication */
	USART_DISABLE();
}
