/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (C) 2013 Atmel Corporation. All rights reserved.
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

#include "conf_usb_host.h"
#include <asf.h>
#include "board_monitor.h"

/* LED backlight control */
#define backlight_on() ioport_set_pin_level(LCD_BL_GPIO,LCD_BL_ACTIVE_LEVEL)
#define backlight_off() ioport_set_pin_level(LCD_BL_GPIO,LCD_BL_INACTIVE_LEVEL)
#define backlight_toggle() ioport_toggle_pin_level(LCD_BL_GPIO)

/* Wakeup pin is PB0 (PC3, EIC5) */
#define UI_WAKEUP_IRQN         EIC_5_IRQn
#define UI_WAKEUP_IRQ_LEVEL    5
#define UI_WAKEUP_EIC_LINE     GPIO_PUSH_BUTTON_EIC_LINE
#define UI_WAKEUP_HANDLER      button_handler
#define UI_WAKEUP_BPM_SRC      BPM_BKUPWEN_EIC

/**
 * \name Internal routines to manage asynchronous interrupt pin change
 * This interrupt is connected to a switch and allows to wakeup CPU in low sleep
 * mode.
 * This wakeup the USB devices connected via a downstream resume.
 * @{
 */
static void ui_enable_asynchronous_interrupt(void);
static void ui_disable_asynchronous_interrupt(void);

/**
 * \brief Interrupt handler for interrupt pin change
 */
static void UI_WAKEUP_HANDLER(void)
{
	sysclk_enable_peripheral_clock(EIC);
	if (eic_line_interrupt_is_pending(EIC, UI_WAKEUP_EIC_LINE)) {
		eic_line_clear_interrupt(EIC, UI_WAKEUP_EIC_LINE);
		if (uhc_is_suspend()) {
			ui_disable_asynchronous_interrupt();

			/* Wakeup host and device */
			uhc_resume();
		}
	}

	sysclk_disable_peripheral_clock(EIC);
}

/**
 * \brief Initializes and enables interrupt pin change
 */
static void ui_enable_asynchronous_interrupt(void)
{
	/* Initialize EIC for button wakeup */
	sysclk_enable_peripheral_clock(EIC);
	struct eic_line_config eic_opt = {
		.eic_mode = EIC_MODE_EDGE_TRIGGERED,
		.eic_edge = EIC_EDGE_FALLING_EDGE,
		.eic_level = EIC_LEVEL_LOW_LEVEL,
		.eic_filter = EIC_FILTER_DISABLED,
		.eic_async = EIC_ASYNCH_MODE
	};
	eic_enable(EIC);
	eic_line_set_config(EIC, UI_WAKEUP_EIC_LINE, &eic_opt);
	eic_line_set_callback(EIC, UI_WAKEUP_EIC_LINE, UI_WAKEUP_HANDLER,
			UI_WAKEUP_IRQN, UI_WAKEUP_IRQ_LEVEL);
	eic_line_enable(EIC, UI_WAKEUP_EIC_LINE);
	eic_line_enable_interrupt(EIC, UI_WAKEUP_EIC_LINE);

	/* EIC can wakeup the device */
	bpm_enable_wakeup_source(BPM, (1 << UI_WAKEUP_BPM_SRC));
	/* EIC can wake the device from backup mode */
	bpm_enable_backup_pin(BPM, 1 << UI_WAKEUP_EIC_LINE);
}

/**
 * \brief Disables interrupt pin change
 */
static void ui_disable_asynchronous_interrupt(void)
{
	eic_line_disable_interrupt(EIC, UI_WAKEUP_EIC_LINE);
	bpm_disable_wakeup_source(BPM, (1 << UI_WAKEUP_BPM_SRC));
	bpm_disable_backup_pin(BPM, 1 << UI_WAKEUP_EIC_LINE);
	sysclk_disable_peripheral_clock(EIC);
}

/*! @} */

/**
 * \name Main user interface functions
 * @{
 */
void ui_init(void)
{
	/* Initialize LEDs */
	LED_Off(LED0);
	/* Initialize backlight */
	backlight_off();
	/* Initialize Board Monitor */
	bm_init();
	bm_mouse_pointer_ctrl(false);
}

void ui_usb_mode_change(bool b_host_mode)
{
	if (b_host_mode) {
		LED_On(LED0);
	} else {
		LED_Off(LED0);
	}
}

/*! @} */

/**
 * \name Host mode user interface functions
 * @{
 */

/*! Status of device enumeration */
static uhc_enum_status_t ui_enum_status = UHC_ENUM_DISCONNECT;
/*! Blink frequency depending on device speed */
static uint16_t ui_device_speed_blink;
/*! Notify the presence of a USB device mouse */
static bool ui_hid_mouse_plug = false;
/*! Notify the presence of a USB device MSC */
static bool ui_msc_plug = false;
/*! Status of the MSC test */
static bool ui_test_done;
/*! Result of the MSC test */
static bool ui_test_result;
/*! Manages device mouse moving */
static int16_t bm_x, bm_y;
/*! Manages device mouse button down */
static uint8_t ui_nb_down = 0;

void ui_usb_vbus_change(bool b_vbus_present)
{
	UNUSED(b_vbus_present);
}

void ui_usb_vbus_error(void)
{
}

void ui_usb_connection_event(uhc_device_t *dev, bool b_present)
{
	UNUSED(dev);
	if (!b_present) {
		LED_On(LED0);
		backlight_off();
		ui_enum_status = UHC_ENUM_DISCONNECT;
		bm_mouse_pointer_ctrl(false);
	}
}

void ui_usb_enum_event(uhc_device_t *dev, uhc_enum_status_t status)
{
	UNUSED(dev);
	ui_enum_status = status;
	if (ui_enum_status == UHC_ENUM_SUCCESS) {
		bm_x = (128 / 2) * 8;
		bm_y = (62 / 2) * 8;
		switch (dev->speed) {
		case UHD_SPEED_HIGH:
			ui_device_speed_blink = 250;
			break;
		case UHD_SPEED_FULL:
			ui_device_speed_blink = 500;
			break;
		case UHD_SPEED_LOW:
		default:
			ui_device_speed_blink = 1000;
			break;
		}
		ui_test_done = false;
	}
}

void ui_uhi_hid_mouse_change(uhc_device_t * dev, bool b_plug)
{
	UNUSED(dev);
	ui_hid_mouse_plug = b_plug;
	bm_mouse_pointer_ctrl(b_plug);
}

void ui_uhi_msc_change(uhc_device_t * dev, bool b_plug)
{
	UNUSED(dev);
	ui_msc_plug = b_plug;
}

void ui_usb_wakeup_event(void)
{
	ui_disable_asynchronous_interrupt();
}

void ui_usb_sof_event(void)
{
	bool b_btn_state;
	static bool btn_suspend = false;
	static uint16_t counter_sof = 0;
	static uint16_t counter_sof_move_refresh = 0;

	if (ui_enum_status == UHC_ENUM_SUCCESS) {
		/* Display device enumerated and in active mode */
		if (++counter_sof > ui_device_speed_blink) {
			counter_sof = 0;
			if (ui_hid_mouse_plug || ui_msc_plug) {
				LED_Toggle(LED0);
			} else {
				LED_On(LED0);
			}
			if (ui_test_done && !ui_test_result) {
				/* Test fail then blink BL */
				backlight_toggle();
			}
		}
		/* Scan button to enter in suspend mode and remote wakeup */
		b_btn_state = (!ioport_get_pin_level(GPIO_PUSH_BUTTON_0)) ?
				true : false;
		if (b_btn_state != btn_suspend) {
			/* Button have changed */
			btn_suspend = b_btn_state;
			if (b_btn_state) {
				/* Button has been pressed */
				LED_Off(LED0);
				backlight_off();
				ui_enable_asynchronous_interrupt();
				uhc_suspend(true);
				return;
			}
		}

		/* Move the remote mouse pointer on the Board Monitor screen */
		if (++counter_sof_move_refresh > 100) {
			counter_sof_move_refresh = 0;
			bm_mouse_pointer_move(bm_x / 8, bm_y / 8);
		}

		/* Power on a LED when the mouse button down */
		if (ui_nb_down) {
			LED_On(LED0);
		}
	}
}

static void ui_uhi_hid_mouse_btn(bool b_state)
{
	UNUSED(b_state);
}

void ui_test_flag_reset(void)
{
	backlight_off();
	ui_test_done = false;
}

void ui_test_finish(bool b_success)
{
	ui_test_done = true;
	ui_test_result = b_success;
	if (b_success) {
		backlight_on();
	}
}
/*! @} */

/*! \name Callback to mange the HID mouse events
 *  @{ */
void ui_uhi_hid_mouse_btn_left(bool b_state)
{
	ui_uhi_hid_mouse_btn(b_state);
}

void ui_uhi_hid_mouse_btn_right(bool b_state)
{
	ui_uhi_hid_mouse_btn(b_state);
}

void ui_uhi_hid_mouse_btn_middle(bool b_state)
{
	ui_uhi_hid_mouse_btn(b_state);
}

void ui_uhi_hid_mouse_move(int8_t x, int8_t y, int8_t scroll)
{
	UNUSED(scroll);
	bm_x += x;
	bm_y += y;
	bm_x = Max(bm_x, 0);
	bm_x = Min(bm_x, 126 * 8);
	bm_y = Max(bm_y, 0);
	bm_y = Min(bm_y, 62 * 8);
}
/*! @} */

/*! \name Callback to show the MSC read and write access
 *  @{ */
void ui_start_read(void)
{
	LED_On(LED0);
}

void ui_stop_read(void)
{
	LED_Off(LED0);
}

void ui_start_write(void)
{
	LED_On(LED0);
}

void ui_stop_write(void)
{
	LED_Off(LED0);
}

/*! @} */

/**
 * \defgroup UI User Interface
 *
 * Jumper setup on SAM4L_EK :
 * - Connect PA06 and usb (VBus detect)
 * - Connect PB05 and usb (ID detect)
 * - Connect PC07 and usb (VBus error detect)
 * - Connect PC08 and usb (VBus control)
 *
 * Human interface on SAM4L_EK :
 * - Led 0 is on when it's host and there is no device connected
 * - Led 0 blinks when a device is enumerated and HID or MSC interface enabled
 *   - The blink is slow (1s) with low speed device
 *   - The blink is normal (0.5s) with full speed device
 *   - The blink is fast (0.25s) with high speed device
 * - Led 0 is on when a read or write access is on going
 * - The mouse on board monitor display moves when the mouse move
 * - LED backlight is on when a LUN test is success
 * - LED backlight blinks when a LUN test is unsuccess
 * - Button PB0 allows to enter the device in suspend mode with remote wakeup
 *   feature authorized
 * - Only PB0 button can be used to wakeup USB device in suspend mode
 */
