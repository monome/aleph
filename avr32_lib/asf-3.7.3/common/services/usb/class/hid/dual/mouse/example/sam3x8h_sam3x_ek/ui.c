/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2012 - 2013 Atmel Corporation. All rights reserved.
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
#include "conf_board.h"
#include "conf_usb_host.h"
#include "conf_usb.h"

/* Wakeup pin is RIGHT CLICK (fast wakeup 14) */
#define  RESUME_PMC_FSTT (PMC_FSMR_FSTT14)
#define  RESUME_PIN      (GPIO_PUSH_BUTTON_2)
#define  RESUME_PIO      (PIN_PUSHBUTTON_2_PIO)
#define  RESUME_PIO_ID   (PIN_PUSHBUTTON_2_ID)
#define  RESUME_PIO_MASK (PIN_PUSHBUTTON_2_MASK)
#define  RESUME_PIO_ATTR (PIN_PUSHBUTTON_2_ATTR)

/*! Current USB mode */
static bool ui_b_host_mode = false;

/**
 * \name Internal routines to manage asynchronous interrupt pin change
 * This interrupt is connected to a switch and allows to wakeup CPU in low sleep
 * mode.
 * In USB device mode, this wakeup the USB host via a upstream resume.
 * In USB host mode, this wakeup the USB devices connected via a downstream
 * resume.
 * @{
 */
static void ui_enable_asynchronous_interrupt(void);
static void ui_disable_asynchronous_interrupt(void);

/* Interrupt on "pin change" from RIGHT CLICK to do wakeup on USB
 *  Note:
 *  This interrupt is enable when the USB host enable remotewakeup feature
 *  This interrupt wakeup the CPU if this one is in idle mode */
static void ui_wakeup_handler(uint32_t id, uint32_t mask)
{
	if (RESUME_PIO_ID == id && RESUME_PIO_MASK == mask) {
		if (ui_b_host_mode) {
			if (!uhc_is_suspend()) {
				/* USB is not in suspend mode
				 *  Let's interrupt enable. */
				return;
			}

			ui_disable_asynchronous_interrupt();

			/* Wakeup the devices connected */
			pmc_wait_wakeup_clocks_restore(uhc_resume);
		} else {
			/* In device mode, wakeup the USB host. */
			pmc_wait_wakeup_clocks_restore(udc_remotewakeup);
		}
	}
}

/**
 * \brief Initializes and enables interrupt pin change
 */
static void ui_enable_asynchronous_interrupt(void)
{
	/* Enable interrupt for button pin */
	pio_get_interrupt_status(RESUME_PIO);
	pio_enable_pin_interrupt(RESUME_PIN);
	/* Enable fast wakeup for button pin */
	pmc_set_fast_startup_input(RESUME_PMC_FSTT);
}

/**
 * \brief Disables interrupt pin change
 */
static void ui_disable_asynchronous_interrupt(void)
{
	/* Disable interrupt for button pin */
	pio_disable_pin_interrupt(RESUME_PIN);
	pio_get_interrupt_status(RESUME_PIO);
	/* Enable fast wakeup for button pin */
	pmc_clr_fast_startup_input(RESUME_PMC_FSTT);
}

/*! @} */

/**
 * \name Main user interface functions
 * @{
 */
void ui_init(void)
{
	/* Enable PIO clock for button inputs */
	pmc_enable_periph_clk(RESUME_PIO_ID);
	pmc_enable_periph_clk(ID_PIOE);
	/* Set handler for wakeup */
	pio_handler_set(RESUME_PIO, RESUME_PIO_ID, RESUME_PIO_MASK,
			RESUME_PIO_ATTR, ui_wakeup_handler);
	/* Enable IRQ for button (PIOB) */
	NVIC_EnableIRQ((IRQn_Type)RESUME_PIO_ID);
	pio_configure_pin(RESUME_PIN, RESUME_PIO_ATTR);

	/* Initialize LEDs */
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	LED_Off(LED2_GPIO);
	LED_Off(LED3_GPIO);
}

void ui_usb_mode_change(bool b_host_mode)
{
	ui_init();
	ui_b_host_mode = b_host_mode;
	if (b_host_mode) {
		LED_On(LED3_GPIO);
	} else {
		LED_Off(LED3_GPIO);
	}
}

/*! @} */

/**
 * \name Host mode user interface functions
 * @{
 */

/*! Status of device enumeration */
static uhc_enum_status_t ui_host_enum_status = UHC_ENUM_DISCONNECT;
/*! Manages device mouse moving */
static int8_t ui_host_x, ui_host_y, ui_host_scroll;

void ui_host_vbus_change(bool b_vbus_present)
{
	UNUSED(b_vbus_present);
}

void ui_host_vbus_error(void)
{
}

void ui_host_connection_event(uhc_device_t *dev, bool b_present)
{
	(void)dev;
	if (b_present) {
		LED_On(LED1_GPIO);
	} else {
		LED_Off(LED1_GPIO);
		ui_host_enum_status = UHC_ENUM_DISCONNECT;
	}
}

void ui_host_enum_event(uhc_device_t *dev, uhc_enum_status_t status)
{
	(void)dev;
	ui_host_enum_status = status;
	if (ui_host_enum_status == UHC_ENUM_SUCCESS) {
		ui_host_x = 0, ui_host_y = 0, ui_host_scroll = 0;
	}
}

void ui_host_wakeup_event(void)
{
	ui_disable_asynchronous_interrupt();
}

void ui_host_sof_event(void)
{
	bool b_btn_state;
	static bool btn_suspend_and_remotewakeup = false;
	static uint16_t counter_sof = 0;

	if (ui_host_enum_status == UHC_ENUM_SUCCESS) {
		/* Display device enumerated and in active mode */
		if (++counter_sof == 500) {
			counter_sof = 0;
			LED_Toggle(LED1_GPIO);
		}

		/* Scan button to enter in suspend mode and remote wakeup */
		b_btn_state = (!gpio_pin_is_high(GPIO_PUSH_BUTTON_1)) ?
				true : false;
		if (b_btn_state != btn_suspend_and_remotewakeup) {
			/* Button have changed */
			btn_suspend_and_remotewakeup = b_btn_state;
			if (b_btn_state) {
				/* Button has been pressed */
				LED_Off(LED0_GPIO);
				LED_Off(LED1_GPIO);
				LED_Off(LED2_GPIO);
				ui_enable_asynchronous_interrupt();
				uhc_suspend(true);
				return;
			}
		}

		/* Power on a LED when the mouse move */
		if (!ui_host_x && !ui_host_y && !ui_host_scroll) {
			LED_Off(LED0_GPIO);
		} else {
			ui_host_x = ui_host_y = ui_host_scroll = 0;
			LED_On(LED0_GPIO);
		}
	}
}

static void ui_host_hid_mouse_btn(bool b_state)
{
	static uint8_t nb_down = 0;
	if (b_state) {
		nb_down++;
	} else {
		nb_down--;
	}

	if (nb_down) {
		LED_On(LED2_GPIO);
	} else {
		LED_Off(LED2_GPIO);
	}
}

void ui_host_hid_mouse_btn_left(bool b_state)
{
	ui_host_hid_mouse_btn(b_state);
}

void ui_host_hid_mouse_btn_right(bool b_state)
{
	ui_host_hid_mouse_btn(b_state);
}

void ui_host_hid_mouse_btn_middle(bool b_state)
{
	ui_host_hid_mouse_btn(b_state);
}

void ui_host_hid_mouse_move(int8_t x, int8_t y, int8_t scroll)
{
	ui_host_x = x;
	ui_host_y = y;
	ui_host_scroll = scroll;
}

/*! @} */

/**
 * \name Device mode user interface functions
 * @{
 */
#define  MOUSE_MOVE_RANGE  3
static bool ui_device_b_mouse_enable = false;

void ui_device_suspend_action(void)
{
	ui_init();
}

void ui_device_resume_action(void)
{
	LED_On(LED1_GPIO);
}

void ui_device_remotewakeup_enable(void)
{
	ui_enable_asynchronous_interrupt();
}

void ui_device_remotewakeup_disable(void)
{
	ui_disable_asynchronous_interrupt();
}

bool ui_device_mouse_enable(void)
{
	ui_device_b_mouse_enable = true;
	return true;
}

void ui_device_mouse_disable(void)
{
	ui_device_b_mouse_enable = false;
}

void ui_device_sof_action(void)
{
	uint16_t framenumber;
	static uint8_t cpt_sof = 0;

	if (!ui_device_b_mouse_enable) {
		return;
	}

	framenumber = udd_get_frame_number();
	if ((framenumber % 1000) == 0) {
		LED_On(LED2_GPIO);
	}

	if ((framenumber % 1000) == 500) {
		LED_Off(LED2_GPIO);
	}

	/* Scan process running each 2ms */
	cpt_sof++;
	if (2 > cpt_sof) {
		return;
	}

	cpt_sof = 0;

	/* Look touch sensor activity for the X and Y events */
	if (!gpio_pin_is_high(GPIO_PUSH_BUTTON_1)) {
		udi_hid_mouse_moveX(-MOUSE_MOVE_RANGE);
	}

	if (!gpio_pin_is_high(GPIO_PUSH_BUTTON_2)) {
		udi_hid_mouse_moveX(MOUSE_MOVE_RANGE);
	}
}

/*! @} */

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM3X_EK :
 * - Led 3 (D5, red) is on in host mode (when USB OTG cable is pluged)
 * - device mode:
 *   - Led 1 (D4, green) is on when USB line is in IDLE mode, and off in SUSPEND
 *     mode
 *   - Led 2 (D3, amber) blinks when USB Host have checked and enabled HID mouse
 *     interface
 *   - Mouse buttons are not linked.
 *   - The LEFT CLICK (BP5) and RIGHT CLICK (BP4) are used to move mouse on the
 *     axis X.
 * - host mode:
 *   - Led 1 is continuously on when a device is connected
 *   - Led 1 blinks when the device is enumerated and USB in idle mode
 *   - Led 2 is on when a HID mouse button is pressed
 *   - Led 0 (D2, blue) is on when the mouse move
 * - LEFT CLICK (BP5) allows to enter the device in suspend mode with remote
 *   wakeup feature authorized
 * - Only RIGHT CLICK (BP4) can be used to wakeup USB device in suspend mode
 */
