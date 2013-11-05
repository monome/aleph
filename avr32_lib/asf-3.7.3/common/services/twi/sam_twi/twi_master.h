/**
 * \file
 *
 * \brief TWI Master driver for SAM.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
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

#ifndef _TWI_MASTER_H_
#define _TWI_MASTER_H_

#include "twi.h"
#include "sysclk.h"

typedef Twi *twi_master_t;
typedef twi_options_t twi_master_options_t;
typedef twi_packet_t twi_package_t;

static inline uint32_t twi_master_setup(twi_master_t p_twi,
		twi_master_options_t *p_opt)
{
	p_opt->master_clk = sysclk_get_cpu_hz();
	p_opt->smbus      = 0;

	if (p_twi == TWI0) {
		sysclk_enable_peripheral_clock(ID_TWI0);
	} else if (p_twi == TWI1) {
		sysclk_enable_peripheral_clock(ID_TWI1);
	} else {
		// Do Nothing
	}

	return (twi_master_init(p_twi, p_opt));
}

#define twi_master_enable(p_twi)   twi_enable_master_mode(p_twi)
#define twi_master_disable(p_twi)  twi_disable_master_mode(p_twi)

#endif // _TWI_MASTER_H_
