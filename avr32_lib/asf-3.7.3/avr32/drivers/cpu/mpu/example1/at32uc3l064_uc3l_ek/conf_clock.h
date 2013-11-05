/**
 * \file
 *
 * \brief Clock configuration
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
#ifndef CONF_CLOCK_H_INCLUDED
#define CONF_CLOCK_H_INCLUDED

/**
 * \note Clock source - DFLL is used to generate 12MHz CPU clock
 */
// ===== System Clock Source Options
#define CONFIG_SYSCLK_SOURCE          SYSCLK_SRC_DFLL

// ===== DFLL Options
#define CONFIG_DFLL0_SOURCE           GENCLK_SRC_RCSYS
/*
 * DFLL min output frequency is 40MHz. Make sure that MUL and DIV doesn't
 * result in an output less than 40MHz frequency.
 */
#define CONFIG_DFLL0_MUL              (48000000UL / OSC_RCSYS_NOMINAL_HZ)
#define CONFIG_DFLL0_DIV              1

// ===== System Clock Bus Division Options to result in 12MHz CPU frequency
#define CONFIG_SYSCLK_CPU_DIV         2 /* Fcpu = Fsys/(2 ^ CPU_div) */
#define CONFIG_SYSCLK_PBA_DIV         2 /* Fpba = Fsys/(2 ^ PBA_div) */
#define CONFIG_SYSCLK_PBB_DIV         2 /* Fpbb = Fsys/(2 ^ PBB_div) */

// ===== Peripheral Clock Management Options-> Enable USART3 for trace messages
#define CONFIG_SYSCLK_INIT_PBAMASK  (1 << SYSCLK_USART3)

#endif /* CONF_CLOCK_H_INCLUDED */
