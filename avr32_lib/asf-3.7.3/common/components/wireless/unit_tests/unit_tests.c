/**
 * \file
 *
 * \brief AT86RFx Wireless Module Unit Test.
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

/*! \mainpage
 * \section intro Introduction
 * This is the unit test application for the \ref at86rfx.h "AT86RFx component".
 * It consists of test cases for the following:
 * - Module initialization
 * - Read/write module register
 * - Frame transmission
 *
 * \section files Main Files
 * - \ref unit_tests.c: AT86RFx Wireless unit tests application.
 * - \ref at86rfx_driver.c: The AT86RFx module driver
 * - \ref at86rfx_driver.h: The AT86RFx module driver header
 * - \ref conf_test.h: test configuration file
 * - \ref conf_board.h: board initialization process configuration
 * - \ref conf_clock.h: clock specific initialization
 * - \ref conf_sleepmgr.h: sleep mode specific configuration
 * - \ref conf_spi_master.h: SPI master mode specific configuration
 * - \ref conf_at86rfx.h: AT86RFx module driver configuration (including SPI
 * service selection)
 * - \ref conf_usb.h: USB specific configuration
 *
 * \section deviceinfo Device Info
 * All AVR devices can be used. This application has been tested
 * with the following setup:
 *   - RZ600 Evaluation kits with a AT86RFx connected to an SPI interface.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

#include <stdint.h>
#include <stdbool.h>
#include <asf.h>
#include <string.h>
#include "board.h"
#include "sysclk.h"
#include "conf_usb.h"
#include "stdio_usb.h"
#include "at86rfx_driver.h"
#include "conf_test.h"
#include "unit_tests.h"


static struct test_case *temp_test;

/**
 * \brief Run Wireless Module unit tests
 *
 * Initializes the clock system, board and USB.
 * Then runs the wireless task continuously.
 */
int main(void)
{
	// Initialize the board and all the peripheral required
	sysclk_init();
	board_init();

	// Initialize interrupt vector table support.
	irq_initialize_vectors();

	// Enable interrupts
	cpu_irq_enable();

	// Enable interrupts
	stdio_usb_init();

	while (1) {
		at86rfx_task();
	};
}

/**
 * \brief Performs a initialization check on AT86RFx module
 *
 * This function will simply test the output of the function
 * \ref at86rfx_init and returns an error in case of failure.
 *
 * \param test Current test case.
 */
static void run_at86rfx_init_test(const struct test_case *test)
{
	at86rfx_retval_t status;

	status = at86rfx_init();
	test_assert_true(test, status == AT86RFX_SUCCESS,
			"Error AT86RFx initialization failed");
}

/**
 * \brief Test the read and write of register on AT86RFx module
 *
 * This function will test the read and write functionalities of register
 * It will first write a known value (state) to register and reads the
 * register to confirm the same value.
 *
 * \param test Current test case.
 */
static void run_at86rfx_reg_access_test(const struct test_case *test)
{
	bool status;
	trx_cmd_t value;

	pal_trx_reg_write(RG_TRX_STATE, CMD_RX_ON);

	value = (trx_cmd_t) pal_trx_reg_read(RG_TRX_STATE);
	status = (CMD_RX_ON == value) ? true : false;
	test_assert_true(test, status == true,
			"Error Read/write AT86RFx register access failed");
}

/**
 * \brief Test the frame transmission on AT86RFx module
 *
 * This function will form a frame with length followed by payload.
 * Then call the \ref at86rfx_tx_frame for frame transmission
 * \ref at86rfx_tal_tx_status_cb gives the status of this transmission
 *
 * \param test Current test case.
 */
static void run_at86rfx_tx_test(const struct test_case *test)
{
	/* Buffer to hold frame to be transmitted. */
	uint8_t tx_buffer[20] = " Test frame";

	/* FCS length has to be added to length. */
	tx_buffer[0] = sizeof(" Test frame") + FCS_LEN;

	temp_test = (struct test_case *)test;

	/* AT86RFx API to transmit the frame. */
	at86rfx_tx_frame(tx_buffer);
}

void at86rfx_tal_tx_status_cb(uint8_t status)
{
	test_assert_true(temp_test, status == AT86RFX_SUCCESS,
			"Error AT86RFx frame transmission failed");
}

void main_cdc_set_dtr(bool b_enable)
{
	if (b_enable) {
		DEFINE_TEST_CASE(at86rfx_init_test, NULL, run_at86rfx_init_test,
				NULL, "AT86RFx initialization");
		DEFINE_TEST_CASE(at86rfx_reg_access_test, NULL,
				run_at86rfx_reg_access_test, NULL,
				"Read/write AT86RFx register access");
		DEFINE_TEST_CASE(at86rfx_tx_test, NULL, run_at86rfx_tx_test,
				NULL, "AT86RFx frame transmission");

		// Put test case addresses in an array.
		DEFINE_TEST_ARRAY(at86rfx_tests) = {
		&at86rfx_init_test,
					&at86rfx_reg_access_test,
					&at86rfx_tx_test};

		// Define the test suite.
		DEFINE_TEST_SUITE(at86rfx_suite, at86rfx_tests,
				"AT86RFx component unit test suite");

		// Run all tests in the test suite.
		test_suite_run(&at86rfx_suite);
	} else {

	}
}
