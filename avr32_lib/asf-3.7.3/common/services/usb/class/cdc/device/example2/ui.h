/**
 * \file
 *
 * \brief Common User Interface for CDC application
 *
 * Copyright (c) 2011 Atmel Corporation. All rights reserved.
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

#ifndef _UI_H_
#define _UI_H_

/*! \brief Called at firmware startup
 */
void ui_init(void);

/*! \brief Called before enter in powerdown mode
 */
void ui_powerdown(void);

/*! \brief Called when CPU exit of powerdown mode
 */
void ui_wakeup(void);

/*! \brief Called when communication port is opened
 */
void ui_com_open(uint8_t port);

/*! \brief Called when communication port is closed
 */
void ui_com_close(uint8_t port);

/*! \brief Called when a data is received on CDC
 */
void ui_com_rx_start(void);

/*! \brief Called when a data is received on port com
 */
void ui_com_tx_start(void);

/*! \brief Called when all data pending are sent on port com
 */
void ui_com_rx_stop(void);

/*! \brief Called when all data pending are sent on CDC
 */
void ui_com_tx_stop(void);

/*! \brief Called when a communication error occur
 */
void ui_com_error(void);

/*! \brief Called when a overflow occur
 */
void ui_com_overflow(void);

/*! \brief This process is called each 1ms
 * It is called only if the USB interface is enabled.
 *
 * \param framenumber  Current frame number
 */
void ui_process(uint16_t framenumber);

#endif // _UI_H_
