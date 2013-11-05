/**
 * \file
 *
 * \brief Main functions for USB host vendor example
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
#include "conf_usb_host.h"
#include "ui.h"
#include "main.h"
#include <string.h>

#define MAIN_VENDOR_LOOPBACK_SIZE (1024)
#define MAIN_VENDOR_ISO_SIZE_1    (MAIN_VENDOR_LOOPBACK_SIZE/2) // 1 invoke
#define MAIN_VENDOR_ISO_SIZE_2    \
	((MAIN_VENDOR_LOOPBACK_SIZE-MAIN_VENDOR_ISO_SIZE_1)/4) // 4 invokes

//! Output buffer for vendor class test
COMPILER_WORD_ALIGNED
static uint8_t main_vendor_buf_out[MAIN_VENDOR_LOOPBACK_SIZE];

//! Input buffer for vendor class test
COMPILER_WORD_ALIGNED
static uint8_t main_vendor_buf_in[MAIN_VENDOR_LOOPBACK_SIZE];

//! Indicate that the test can be started
static volatile bool main_b_usb_start_test = false;

//! The control transfer is in progress
static volatile bool main_b_control_busy;

//! The (bulk/interrupt/iso) IN transfer is in progress
static volatile bool main_b_in_busy;
//! The (bulk/interrupt/iso) OUT transfer is in progress
static volatile bool main_b_out_busy;

//! The transfer status
static volatile uhd_trans_status_t main_transfer_status;

//! Index for ISO IN data transfer
static uint32_t main_vendor_iso_in_index;
//! Index for ISO OUT data transfer
static uint32_t main_vendor_iso_out_index;

static void main_init_buffers(void);
static int  main_cmp_buffers(void);
static bool main_control_is_available(void);
static bool main_loop_back_control(void);
static bool main_loop_back_int(void);
static bool main_loop_back_bulk(void);
static bool main_loop_back_iso(void);

//! If the test is available (the endpoint is found on attached device)
static bool (*loop_back_tests_is_available[])(void) = {
	main_control_is_available,
	uhi_vendor_bulk_is_available,
	uhi_vendor_int_is_available,
	uhi_vendor_iso_is_available,
};

//! The test function entries
static bool (*loop_back_tests[])(void) = {
	main_loop_back_control,
	main_loop_back_bulk,
	main_loop_back_int,
	main_loop_back_iso,
};

//! Number of tests available
#define NB_TESTS    (sizeof(loop_back_tests) / sizeof(loop_back_tests[0]))

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	board_init();
	ui_init();

	// Start USB host stack
	uhc_start();

	// The USB management is entirely managed by interrupts.
	// As a consequence, the user application does only have :
	// - to play with the power modes
	// - to test all endpoint pares enumerated
	while (true) {
		sleepmgr_enter_sleep();
		if (main_b_usb_start_test) {
			main_b_usb_start_test = false;
			bool main_usb_test_fail = false;

			for (uint8_t test = 0; test < NB_TESTS; test ++) {
				if (!loop_back_tests_is_available[test]()) {
					// Test not available
					continue;
				}
				main_init_buffers();
				if (!loop_back_tests[test]()) {
					// USB communication error
					main_usb_test_fail = true;
					break;
				}
				if (main_cmp_buffers()) {
					// Data compare error
					main_usb_test_fail = true;
					break;
				}
			}
			// Show result
			if (!main_usb_test_fail) {
				// All tests passed
				ui_test_finish(true);
			} else {
				// Fail
				ui_test_finish(false);
			}
		}
	}
}

void main_usb_sof_event(void)
{
	ui_usb_sof_event();
}

void main_usb_vendor_change(uhc_device_t * dev, bool b_present)
{
	UNUSED(dev);
	main_b_usb_start_test = b_present;
}

/** \brief Initialize test buffers
 */
static void main_init_buffers(void)
{
	uint32_t i;
	// Fill buffer OUT
	for (i = 0; i < MAIN_VENDOR_LOOPBACK_SIZE; i += 4) {
		main_vendor_buf_out[i + 0] = (i >> 24) & 0xFF;
		main_vendor_buf_out[i + 1] = (i >> 16) & 0xFF;
		main_vendor_buf_out[i + 2] = (i >>  8) & 0xFF;
		main_vendor_buf_out[i + 3] = (i >>  0) & 0xFF;
	}
	// Reset buffer IN
	memset(main_vendor_buf_in, 0x55, MAIN_VENDOR_LOOPBACK_SIZE);
}

/** \brief Compare output buffer with input buffer
 *
 * \return 0 if buffer is equal, else -1
 */
static int main_cmp_buffers(void)
{
	if (0!=memcmp( main_vendor_buf_out, main_vendor_buf_in,
			MAIN_VENDOR_LOOPBACK_SIZE) ) {
		return -1;
	}
	return 0;
}

/** \brief Check if control test is available (control EP exist)
 *
 * \return always true
 */
static bool main_control_is_available(void)
{
	return true;
}

/** \brief End of setup callback
 *
 * \param add           USB address of the setup request
 * \param status        Transfer status
 * \param payload_trans Number of data transfered during DATA phase
 */
static void main_control_transfer_done (
		usb_add_t add,
		uhd_trans_status_t status,
		uint16_t payload_trans)
{
	UNUSED(add);
	UNUSED(payload_trans);
	main_transfer_status = status;
	main_b_control_busy = false;
}

/** \brief Perform loop back test on control endpoint
 *
 * \return \c 1 if test passed, \c 0 if test failed
 */
static bool main_loop_back_control(void)
{
	main_b_control_busy = true;
	if (!uhi_vendor_control_out_run(main_vendor_buf_out,
			MAIN_VENDOR_LOOPBACK_SIZE,
			main_control_transfer_done)) {
		return false;
	}
	while (main_b_control_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}

	main_b_control_busy = true;
	if (!uhi_vendor_control_in_run(main_vendor_buf_in,
			MAIN_VENDOR_LOOPBACK_SIZE,
			main_control_transfer_done)) {
		return false;
	}
	while (main_b_control_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}
	return true;
}

/** \brief End of transfer callback
 *
 * \param add           USB address used by the transfer
 * \param ep            Endpoint address used by the transfer
 * \param status        Transfer status
 * \param nb_transfered Number of data transfered
 */
static void main_transfer_done (
		usb_add_t add,
		usb_ep_t ep,
		uhd_trans_status_t status,
		iram_size_t nb_transfered)
{
	UNUSED(add);
	UNUSED(nb_transfered);

	main_transfer_status = status;
	if (ep & USB_EP_DIR_IN) {
		main_b_in_busy = false;
	} else {
		main_b_out_busy = false;
	}
}

/** \brief Perform loop back test on control endpoint
 */
static bool main_loop_back_bulk(void)
{

	main_b_out_busy = true;
	if (!uhi_vendor_bulk_out_run(main_vendor_buf_out,
			MAIN_VENDOR_LOOPBACK_SIZE, main_transfer_done)) {
		return false;
	}
	while (main_b_out_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}

	main_b_in_busy = true;
	if (!uhi_vendor_bulk_in_run(main_vendor_buf_in,
			MAIN_VENDOR_LOOPBACK_SIZE, main_transfer_done)) {
		return false;
	}
	while (main_b_in_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}
	return true;
}

/** \brief Perform loop back test on interrupt endpoint
 *
 * \return \c 1 if test passed, \c 0 if test failed
 */
static bool main_loop_back_int(void)
{
	main_b_out_busy = true;
	if (!uhi_vendor_int_out_run(main_vendor_buf_out,
			MAIN_VENDOR_LOOPBACK_SIZE, main_transfer_done)) {
		return false;
	}
	while (main_b_out_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}

	main_b_in_busy = true;
	if (!uhi_vendor_int_in_run(main_vendor_buf_in,
			MAIN_VENDOR_LOOPBACK_SIZE, main_transfer_done)) {
		return false;
	}
	while (main_b_in_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}
	return true;
}

/** \brief End of ISO IN transfer callback
 *
 * \param add           USB address used by the transfer
 * \param ep            Endpoint address used by the transfer
 * \param status        Transfer status
 * \param nb_transfered Number of data transfered
 */
static void main_iso_in_done (
		usb_add_t add,
		usb_ep_t ep,
		uhd_trans_status_t status,
		iram_size_t nb_transfered)
{
	UNUSED(add);
	UNUSED(ep);
	main_vendor_iso_in_index += nb_transfered;
	if (main_vendor_iso_in_index >= MAIN_VENDOR_LOOPBACK_SIZE) {
		main_transfer_status = status;
		main_b_in_busy = false;
	} else {
		uhi_vendor_iso_in_run(
			&main_vendor_buf_in[main_vendor_iso_in_index],
			MAIN_VENDOR_LOOPBACK_SIZE - main_vendor_iso_in_index,
			main_iso_in_done);
	}
}

/** \brief End of ISO OUT transfer callback
 *
 * \param add           USB address used by the transfer
 * \param ep            Endpoint address used by the transfer
 * \param status        Transfer status
 * \param nb_transfered Number of data transfered
 */
static void main_iso_out_done (
		usb_add_t add,
		usb_ep_t ep,
		uhd_trans_status_t status,
		iram_size_t nb_transfered)
{
	UNUSED(add);
	UNUSED(ep);
	UNUSED(status);
	main_vendor_iso_out_index += nb_transfered;
	if (main_vendor_iso_out_index >= MAIN_VENDOR_LOOPBACK_SIZE) {
		main_transfer_status = status;
		main_b_out_busy = false;
	} else {
		uhi_vendor_iso_out_run(
			&main_vendor_buf_out[main_vendor_iso_out_index],
			MAIN_VENDOR_ISO_SIZE_2,
			main_iso_out_done);
	}
}

/** \brief Perform loop back test on ISO endpoint
 *
 * \return \c 1 if test passed, \c 0 if test failed
 */
static bool main_loop_back_iso(void)
{
	main_b_out_busy = true;
	main_vendor_iso_out_index = 0;
	if (!uhi_vendor_iso_out_run(main_vendor_buf_out,
			MAIN_VENDOR_ISO_SIZE_1, main_iso_out_done)) {
		return false;
	}
	main_b_in_busy = true;
	main_vendor_iso_in_index = 0;
	if (!uhi_vendor_iso_in_run(main_vendor_buf_in,
			MAIN_VENDOR_LOOPBACK_SIZE, main_iso_in_done)) {
		return false;
	}
	while (main_b_out_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}
	while (main_b_in_busy);
	if (main_transfer_status != UHD_TRANS_NOERROR) {
		return false;
	}
	return true;
}


/**
 * \mainpage ASF USB host vendor
 *
 * \section intro Introduction
 * This example shows how to implement a USB host vendor
 * on Atmel MCU with USB module.
 *
 * \section startup Startup
 * After loading firmware, connect the board (EVKxx,Xplain,...) to a board
 * that runs USB device vendor example. This example transmit and receives
 * on all endpoints enumerated.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB host stack and MSC modules:
 *   <br>services/usb/
 *   <br>services/usb/uhc/
 *   <br>services/usb/class/vendor/host/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
