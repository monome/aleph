/**
 * \file
 *
 * \brief SD/MMC card unit tests
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

/**
 * \mainpage SD/MMC/SDIO Card Unit tests
 *
 * \section intro Introduction
 * This is the unit test application for the SD/MMC stack.
 * It consists of test cases for the following functionalities:
 * - SD/MMC/SDIO card initialization.
 * - SD/MMC/SDIO card read and write test.
 * The SD/MMC stack supports all MCI/SPI interfaces from AVR and SAM devices.
 *
 * \note Please connect a SD/MMC card on the board before running the test.
 * \section Description
 *
 * The unit tests executes the following sequences:
 * - Wait a card insertion on the first slot of the board
 * - Inilialize an SD, MMC or SDIO card.
 * - If the inserted card is an SD or MMC card, test read/write access.
 * Three modes are tested: SINGLE, MULTIPLE block, and fragmented MULTIPLE block.
 *     - Read data from the card, save it
 *     - Write data into the card
 *     - Read data from the card
 *     - Verify the written data.
 *     - Write the data saved
 *     - Read the data saved
 *     - Verify the written data.
 * - If the inserted card is a SDIO card:
 *   - Inilialize a SDIO card.
 *   - Read and write test on CIA.
 *
 * The example outputs the information through the standard utput (stdio).
 * To know the output used by the board, look in  the conf_test.h file
 * and connect a terminal on the correct port.
 *
 * \section contactinfo Contact Information
 * For further information, visit <a href="http://www.atmel.com/">Atmel</a>.\n
 * Support and FAQ: http://support.atmel.no/
 */

//! \name Unit test configuration
//! @{
/**
 * \def CONF_TEST_UART
 * \brief UART to redirect STDIO
 */
/**
 * \def CONF_TEST_BAUDRATE
 * \brief Baudrate of UART
 */
/**
 * \def CONF_TEST_PARITY
 * \brief Parity mode of UART
 */
//! @}

#include <asf.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_test.h"
#include "sd_mmc_protocol.h"


//! \name Test configurations
//! @{
//! Number of blocks for multiple read or write
#define NB_MULTI_BLOCKS     (2)

//! The value used to generate test data
#define TEST_FILL_VALUE_U32     (0x5500AAFFU)

//! Buffer used to save the sector area used by read/write tests
COMPILER_WORD_ALIGNED
static uint8_t buf_save[SD_MMC_BLOCK_SIZE * NB_MULTI_BLOCKS];

//! Buffer used by read/write tests
COMPILER_WORD_ALIGNED
static uint8_t buf_test[SD_MMC_BLOCK_SIZE * NB_MULTI_BLOCKS];

//! Read and write test length of CIA in bytes
#define TEST_CIA_SIZE           (0x16)

//! Buffer for test SDIO data
COMPILER_WORD_ALIGNED
static uint8_t buf_cia[TEST_CIA_SIZE];
//! @}


/**
 * \brief SD/MMC card read and write test.
 *
 * \param test Current test case.
 */
static void run_sd_mmc_rw_test(const struct test_case *test)
{
	uint32_t i;
	uint32_t last_blocks_addr;
	uint16_t nb_block, nb_trans;
	bool split_tansfer = false;

	/* Compute the last address */
	last_blocks_addr = sd_mmc_get_capacity(0) * (1024/SD_MMC_BLOCK_SIZE) - 50;
	test_assert_true(test, last_blocks_addr > NB_MULTI_BLOCKS,
			"Error: SD/MMC capacity.");

	last_blocks_addr -= NB_MULTI_BLOCKS;
	nb_block = 1;
	
run_sd_mmc_rw_test_next:

	/* Read (save blocks) the last blocks */
	test_assert_true(test, SD_MMC_OK ==
			sd_mmc_init_read_blocks(0, last_blocks_addr, nb_block),
			"Error: SD/MMC initialize read sector(s).");

	for (nb_trans = 0; nb_trans < (split_tansfer? nb_block : 1); nb_trans++) {
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_start_read_blocks(
				&buf_save[nb_trans * SD_MMC_BLOCK_SIZE],
				split_tansfer? 1 : nb_block),
				"Error: SD/MMC start read sector(s).");
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_wait_end_of_read_blocks(),
				"Error: SD/MMC wait end of read sector(s).");
	}

	test_assert_true(test, !sd_mmc_is_write_protected(0),
			"Error: SD/MMC is write protected.");

	/* Fill buffer */
	for (i = 0; i < (SD_MMC_BLOCK_SIZE * nb_block / sizeof(uint32_t)); i++) {
		((uint32_t*)buf_test)[i] = TEST_FILL_VALUE_U32;
	}
	
	/* Write the last blocks */
	test_assert_true(test, SD_MMC_OK ==
			sd_mmc_init_write_blocks(0, last_blocks_addr, nb_block),
			"Error: SD/MMC initialize write sector(s).");

	for (nb_trans = 0; nb_trans < (split_tansfer? nb_block : 1); nb_trans++) {
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_start_write_blocks(
				&buf_test[nb_trans * SD_MMC_BLOCK_SIZE],
				split_tansfer? 1 : nb_block),
				"Error: SD/MMC start write sector(s).");
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_wait_end_of_write_blocks(),
				"Error: SD/MMC wait end of write sector(s).");
	}

	/* Clean buffer */
	for (i = 0; i < (SD_MMC_BLOCK_SIZE * nb_block / sizeof(uint32_t)); i++) {
		((uint32_t*)buf_test)[i] = 0xFFFFFFFF;
	}

	/* Read the last block */
	test_assert_true(test, SD_MMC_OK ==
			sd_mmc_init_read_blocks(0, last_blocks_addr, nb_block),
			"Error: SD/MMC initialize read sector(s).");

	for (nb_trans = 0; nb_trans < (split_tansfer? nb_block : 1); nb_trans++) {
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_start_read_blocks(
				&buf_test[nb_trans * SD_MMC_BLOCK_SIZE],
				split_tansfer? 1 : nb_block),
				"Error: SD/MMC start read sector(s).");
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_wait_end_of_read_blocks(),
				"Error: SD/MMC wait end of read sector(s).");
	}

	/* Check buffer */
	for (i = 0; i < (SD_MMC_BLOCK_SIZE * nb_block / sizeof(uint32_t)); i++) {
		test_assert_true(test,
				((uint32_t*)buf_test)[i] == TEST_FILL_VALUE_U32,
				"Error: SD/MMC verify write operation.");
	}

	/* Write (restore) the last block */
	test_assert_true(test, SD_MMC_OK ==
			sd_mmc_init_write_blocks(0, last_blocks_addr, nb_block),
			"Error: SD/MMC initialize write restore sector(s).");

	for (nb_trans = 0; nb_trans < (split_tansfer? nb_block : 1); nb_trans++) {
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_start_write_blocks(
				&buf_save[nb_trans * SD_MMC_BLOCK_SIZE],
				split_tansfer? 1 : nb_block),
				"Error: SD/MMC start write restore sector(s).");
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_wait_end_of_write_blocks(),
				"Error: SD/MMC wait end of write restore sector(s).");
	}

	/* Read (check restore) the last block */
	test_assert_true(test, SD_MMC_OK ==
			sd_mmc_init_read_blocks(0, last_blocks_addr, nb_block),
			"Error: SD/MMC initialize read sector(s).");

	for (nb_trans = 0; nb_trans < (split_tansfer? nb_block : 1); nb_trans++) {
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_start_read_blocks(
				&buf_test[nb_trans * SD_MMC_BLOCK_SIZE],
				split_tansfer? 1 : nb_block),
				"Error: SD/MMC start read sector(s).");
		test_assert_true(test, SD_MMC_OK ==
				sd_mmc_wait_end_of_read_blocks(),
				"Error: SD/MMC wait end of read sector(s).");
	}

	/* Check buffer restored */
	for (i = 0; i < (SD_MMC_BLOCK_SIZE * nb_block / sizeof(uint32_t)); i++) {
		test_assert_true(test,
				((uint32_t*)buf_test)[i] == ((uint32_t*)buf_save)[i],
				"Error: SD/MMC verify restore operation.");
	}

	if (nb_block == 1) {
		/* Launch second test */
		nb_block = NB_MULTI_BLOCKS;
		goto run_sd_mmc_rw_test_next;
	}
	if (!split_tansfer) {
		/* Launch third test */
		split_tansfer = true;
		goto run_sd_mmc_rw_test_next;
	}
}

/**
 * \brief SDIO card read and write test.
 *
 * \param test Current test case.
 */
static void run_sdio_rw_test(const struct test_case *test)
{
	sd_mmc_err_t err;

	/*
	 * ---- Test direct R/W ----
	 */
	/* Write */
	err = sdio_write_direct(0, SDIO_CIA, SDIO_CCCR_IEN, 0x02);
	test_assert_true(test, err == SD_MMC_OK,
			"Error: SDIO direct write failed\n\r");

	/* Read */
	err = sdio_read_direct(0, SDIO_CIA, SDIO_CCCR_IEN,
			&buf_cia[SDIO_CCCR_IEN]);
	test_assert_true(test, err == SD_MMC_OK,
			"Error: SDIO direct read failed\n\r");

	/* Check */
	test_assert_true(test, buf_cia[SDIO_CCCR_IEN] == 0x02,
			"Error: SDIO direct R/W verification failed\n\r");

	/* Restore data to 0 */
	sdio_write_direct(0, SDIO_CIA, SDIO_CCCR_IEN, 0);

	/*
	 * ---- Test extended R/W ----
	 */
	/* Write */
	buf_cia[SDIO_CCCR_IEN] = 0x3;
	err = sdio_write_extended(0, SDIO_CIA, SDIO_CCCR_IEN, 1,
			&buf_cia[SDIO_CCCR_IEN], 1);
	test_assert_true(test, err == SD_MMC_OK,
			"Error: SDIO extended write failed\n\r");

	/* Read and check */
	err = sdio_read_extended(0, SDIO_CIA, 0, 1, &buf_cia[0],
			TEST_CIA_SIZE);
	test_assert_true(test, err == SD_MMC_OK,
			"Error: SDIO extended read failed\n\r");

	test_assert_true(test, buf_cia[SDIO_CCCR_IEN] == 0x3,
			"Error: SDIO extended R/W verification failed\n\r");

	/* Restore data to 0 */
	sdio_write_direct(0, SDIO_CIA, SDIO_CCCR_IEN, 0);
}

/**
 * \brief SD/MMC stack initialization test.
 *
 * \param test Current test case.
 */
static void run_sd_mmc_init_test(const struct test_case *test)
{
	sd_mmc_err_t err;

	/* Initialize SD MMC stack */
	sd_mmc_init();

	/* Wait card connection */
	do {
		err = sd_mmc_check(0);
	} while (SD_MMC_ERR_NO_CARD == err);

	/* Check if the first state is an initialization */
	test_assert_true(test, err == SD_MMC_INIT_ONGOING,
			"No card initialization phase detected.");

	/* Check if the second step is an installation success */
	test_assert_true(test, sd_mmc_check(0) == SD_MMC_OK,
			"SD/MMC card initialization failed.");
}

/**
 * \brief SD/MMC/SDIO card read and write test.
 *
 * \param test Current test case.
 */
static void run_sd_mmc_sdio_rw_test(const struct test_case *test)
{
	test_assert_true(test, SD_MMC_OK == sd_mmc_check(0),
			"SD/MMC card is not initialized OK.");

	if (sd_mmc_get_type(0) & (CARD_TYPE_SD | CARD_TYPE_MMC)) {
		run_sd_mmc_rw_test(test);
	}
	if (sd_mmc_get_type(0) & CARD_TYPE_SDIO) {
		run_sdio_rw_test(test);
	}
}

/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	const usart_serial_options_t usart_serial_options = {
		.baudrate   = CONF_TEST_BAUDRATE,
		.charlength = CONF_TEST_CHARLENGTH,
		.paritytype = CONF_TEST_PARITY,
		.stopbits   = CONF_TEST_STOPBITS,
	};

	irq_initialize_vectors();
	cpu_irq_enable();

	sysclk_init();
	board_init();
	stdio_serial_init(CONF_TEST_USART, &usart_serial_options);

	/* Define all the test cases */
	DEFINE_TEST_CASE(sd_mmc_init_test, NULL, run_sd_mmc_init_test,
			NULL, "SD/MMC/SDIO card initialization test.");

	DEFINE_TEST_CASE(sd_mmc_rw_test, NULL, run_sd_mmc_sdio_rw_test,
			NULL, "SD/MMC/SDIO card read and write test.");

	/* Put test case addresses in an array */
	DEFINE_TEST_ARRAY(sd_mmc_tests) = {
		&sd_mmc_init_test,
		&sd_mmc_rw_test
	};

	/* Define the test suite */
	DEFINE_TEST_SUITE(sd_mmc_suite, sd_mmc_tests,
			"SD/MMC stack test suite");

	/* Run all tests in the test suite */
	test_suite_run(&sd_mmc_suite);

	while (1) {
		/* Busy-wait forever */
	}
}
