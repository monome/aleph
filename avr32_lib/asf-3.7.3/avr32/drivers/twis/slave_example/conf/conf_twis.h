/*****************************************************************************
 *
 * \file
 *
 * \brief TWIS Configuration File for AVR32 UC3.
 *
 * This file defines a useful set of functions for TWIM on AVR32 devices.
 *
 *****************************************************************************/

/**
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
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
#ifndef _CONF_TWIS_H
#define _CONF_TWIS_H
#include "board.h"

#if BOARD==UC3C_EK
// These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#	if UC3C
#		if !defined(AVR32_TWIS0_GROUP)
#			define AVR32_TWIS0_GROUP         27
#		else
#			warning "Duplicate define(s) to remove from the ASF"
#		endif
#		if !defined(AVR32_TWIS1_GROUP)
#			define AVR32_TWIS1_GROUP         28
#		else
#			warning "Duplicate define(s) to remove from the ASF"
#		endif
#		if !defined(AVR32_TWIS2_GROUP)
#			define AVR32_TWIS2_GROUP         46
#		else
#			warning "Duplicate define(s) to remove from the ASF"
#		endif
#	endif
#	define CONF_TWIS_IRQ_LINE          AVR32_TWIS0_IRQ
#	define CONF_TWIS_IRQ_GROUP         AVR32_TWIS0_GROUP
#	define CONF_TWIS_IRQ_LEVEL         1
#elif BOARD==EVK1104
#	if UC3A3
#		if !defined(AVR32_TWIS0_GROUP)
#			define AVR32_TWIS0_GROUP         27
#		else
#			warning "Duplicate define(s) to remove from the ASF"
#		endif
#		if !defined(AVR32_TWIS1_GROUP)
#			define AVR32_TWIS1_GROUP         28
#		else
#			warning "Duplicate define(s) to remove from the ASF"
#		endif
#	endif
#	define CONF_TWIS_IRQ_LINE          AVR32_TWIS0_IRQ
#	define CONF_TWIS_IRQ_GROUP         AVR32_TWIS0_GROUP
#	define CONF_TWIS_IRQ_LEVEL         1
#elif BOARD==UC3L_EK
#	if UC3L
#		if !defined(AVR32_TWIS0_GROUP)
#			define AVR32_TWIS0_GROUP         22
#		else
#			warning "Duplicate define(s) to remove from the ASF"
#		endif
#		if !defined(AVR32_TWIS1_GROUP)
#			define AVR32_TWIS1_GROUP         23
#		else
#			warning "Duplicate define(s) to remove from the ASF"
#		endif
#	endif
#	define CONF_TWIS_IRQ_LINE          AVR32_TWIS0_IRQ
#	define CONF_TWIS_IRQ_GROUP         AVR32_TWIS0_GROUP
#	define CONF_TWIS_IRQ_LEVEL         1
#else
#	error Board Not Supported
#endif

#endif // _CONF_TWIS_H
