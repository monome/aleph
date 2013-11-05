/**
 * \file
 *
 * \brief Chip-specific oscillator management functions
 *
 * Copyright (c) 2010-2011 Atmel Corporation. All rights reserved.
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
#ifndef CHIP_OSC_H_INCLUDED
#define CHIP_OSC_H_INCLUDED

#include <board.h>

#ifdef __cplusplus
extern "C" {
#endif

// These defines are missing from or wrong in the toolchain header files.
// #11802
#ifdef AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_AGC
#undef AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_AGC
#endif
#define AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_AGC      0x00000001
#ifdef AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_NO_AGC
#undef AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_NO_AGC
#endif
#define AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_NO_AGC   0x00000004
#ifdef AVR32_SCIF_OSCCTRL32_MODE_EXT_CLOCK
#undef AVR32_SCIF_OSCCTRL32_MODE_EXT_CLOCK
#endif
#define AVR32_SCIF_OSCCTRL32_MODE_EXT_CLOCK        0x00000000


/**
 * \weakgroup osc_group
 * @{
 */

//! \name Oscillator identifiers
//@{
#define OSC_ID_OSC0             0       //!< External Oscillator 0
#define OSC_ID_OSC32            1       //!< External 32 kHz oscillator
#define OSC_ID_RC32K            2       //!< Internal 32 kHz RC oscillator
#define OSC_ID_RC120M           3       //!< Internal 120 MHz RC oscillator
#define OSC_ID_RCSYS            4       //!< Internal System RC oscillator
//@}

//! \name OSC0 mode values
//@{
//! External clock connected to XIN
#define OSC_MODE_EXTERNAL       AVR32_SCIF_OSCCTRL0_MODE_EXT_CLOCK
//! Crystal connected to XIN/XOUT
#define OSC_MODE_XTAL           AVR32_SCIF_OSCCTRL0_MODE_CRYSTAL
//@}

//! \name OSC32 mode values
//@{
//! External clock connected to XIN32
#define OSC32_MODE_EXTERNAL     AVR32_SCIF_OSCCTRL32_MODE_EXT_CLOCK
//! Crystal connected to XIN32/XOUT32
#define OSC32_MODE_XTAL         AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_AGC
//! Crystal connected to XIN32/XOUT32 in high current mode
#define OSC32_MODE_XTAL_HC      AVR32_SCIF_OSCCTRL32_MODE_CRYSTAL_NO_AGC
//@}

//! \name OSC0 startup values
//@{
//! 0 cycles
#define OSC_STARTUP_0           AVR32_SCIF_OSCCTRL0_STARTUP_0_RCOSC
//! 64 cycles (560 us)
#define OSC_STARTUP_64          AVR32_SCIF_OSCCTRL0_STARTUP_64_RCOSC
//! 128 cycles (1.1 ms)
#define OSC_STARTUP_128         AVR32_SCIF_OSCCTRL0_STARTUP_128_RCOSC
//! 2048 cycles (18 ms)
#define OSC_STARTUP_2048        AVR32_SCIF_OSCCTRL0_STARTUP_2048_RCOSC
//! 4096 cycles (36 ms)
#define OSC_STARTUP_4096        AVR32_SCIF_OSCCTRL0_STARTUP_4096_RCOSC
//! 8192 cycles (71 ms)
#define OSC_STARTUP_8192        AVR32_SCIF_OSCCTRL0_STARTUP_8192_RCOSC
//! 16384 cycles (142 ms)
#define OSC_STARTUP_16384       AVR32_SCIF_OSCCTRL0_STARTUP_16384_RCOSC
//@}

//! \name OSC32 startup values
//@{
//! 0 cycles
#define OSC32_STARTUP_0         AVR32_SCIF_OSCCTRL32_STARTUP_0_RCOSC
//! 128 cycles (1.1 ms)
#define OSC32_STARTUP_128       AVR32_SCIF_OSCCTRL32_STARTUP_128_RCOSC
//! 8192 cycles (72.3 ms)
#define OSC32_STARTUP_8192      AVR32_SCIF_OSCCTRL32_STARTUP_8192_RCOSC
//! 16384 cycles (143 ms)
#define OSC32_STARTUP_16384     AVR32_SCIF_OSCCTRL32_STARTUP_16384_RCOSC
//! 65536 cycles (570 ms)
#define OSC32_STARTUP_65536     AVR32_SCIF_OSCCTRL32_STARTUP_65536_RCOSC
//! 131072 cycles (1.1 s)
#define OSC32_STARTUP_131072    AVR32_SCIF_OSCCTRL32_STARTUP_131072_RCOSC
//! 262144 cycles (2.3 s)
#define OSC32_STARTUP_262144    AVR32_SCIF_OSCCTRL32_STARTUP_262144_RCOSC
//! 524288 cycles (4.6 s)
#define OSC32_STARTUP_524288    AVR32_SCIF_OSCCTRL32_STARTUP_524288_RCOSC
//@}

/**
 * \def OSC0_STARTUP_TIMEOUT
 * \brief Number of slow clock cycles to wait for OSC0 to start
 *
 * This is the number of slow clock cycles corresponding to
 * OSC0_STARTUP_VALUE with an additional 25% safety margin. If the
 * oscillator isn't running when this timeout has expired, it is assumed
 * to have failed to start.
 */
/**
 * \def OSC0_MODE_VALUE
 * \brief Board-dependent value written to the MODE bitfield of
 * PM_OSCCTRL(0)
 */
/**
 * \def OSC0_STARTUP_VALUE
 * \brief Board-dependent value written to the STARTUP bitfield of
 * PM_OSCCTRL(0)
 */

#if defined(BOARD_OSC0_STARTUP_US)
# if BOARD_OSC0_STARTUP_US == 0
#  define OSC0_STARTUP_VALUE    OSC_STARTUP_0
#  define OSC0_STARTUP_TIMEOUT  8
# elif BOARD_OSC0_STARTUP_US <= 560
#  define OSC0_STARTUP_VALUE    OSC_STARTUP_64
#  define OSC0_STARTUP_TIMEOUT  80
# elif BOARD_OSC0_STARTUP_US <= 1100
#  define OSC0_STARTUP_VALUE    OSC_STARTUP_128
#  define OSC0_STARTUP_TIMEOUT  160
# elif BOARD_OSC0_STARTUP_US <= 18000
#  define OSC0_STARTUP_VALUE    OSC_STARTUP_2048
#  define OSC0_STARTUP_TIMEOUT  2560
# elif BOARD_OSC0_STARTUP_US <= 36000
#  define OSC0_STARTUP_VALUE    OSC_STARTUP_4096
#  define OSC0_STARTUP_TIMEOUT  5120
# elif BOARD_OSC0_STARTUP_US <= 71000
#  define OSC0_STARTUP_VALUE    OSC_STARTUP_8192
#  define OSC0_STARTUP_TIMEOUT  10240
# elif BOARD_OSC0_STARTUP_US <= 142000
#  define OSC0_STARTUP_VALUE    OSC_STARTUP_16384
#  define OSC0_STARTUP_TIMEOUT  20480
# else
#  error BOARD_OSC0_STARTUP_US is too high
# endif
# if BOARD_OSC0_IS_XTAL == true
#  define OSC0_MODE_VALUE       OSC_MODE_XTAL
#  if BOARD_OSC0_HZ < 900000
#   define OSC0_GAIN_VALUE      AVR32_SCIF_OSCCTRL0_GAIN_G0
#  elif BOARD_OSC0_HZ < 3000000
#   define OSC0_GAIN_VALUE      AVR32_SCIF_OSCCTRL0_GAIN_G1
#  elif BOARD_OSC0_HZ < 8000000
#   define OSC0_GAIN_VALUE      AVR32_SCIF_OSCCTRL0_GAIN_G2
#  else
#   define OSC0_GAIN_VALUE      AVR32_SCIF_OSCCTRL0_GAIN_G3
#  endif
# else
#  define OSC0_MODE_VALUE       OSC_MODE_EXTERNAL
# endif
#else
# if defined(BOARD_OSC0_HZ)
#  error BOARD_OSC0_STARTUP_US must be defined by the board code
# endif
# ifdef __DOXYGEN__
#  define OSC0_STARTUP_VALUE     UNDEFINED
#  define OSC0_STARTUP_TIMEOUT   UNDEFINED
#  define OSC0_MODE_VALUE        UNDEFINED
# endif
#endif

#if defined(BOARD_OSC32_STARTUP_US)
# if BOARD_OSC32_STARTUP_US == 0
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_0
# elif BOARD_OSC32_STARTUP_US <= 1100
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_128
# elif BOARD_OSC32_STARTUP_US <= 72300
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_8192
# elif BOARD_OSC32_STARTUP_US <= 143000
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_16384
# elif BOARD_OSC32_STARTUP_US <= 570000
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_65536
# elif BOARD_OSC32_STARTUP_US <= 1100000
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_131072
# elif BOARD_OSC32_STARTUP_US <= 2300000
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_262144
# elif BOARD_OSC32_STARTUP_US <= 4600000
#  define OSC32_STARTUP_VALUE    OSC32_STARTUP_524288
# else
#  error BOARD_OSC32_STARTUP_US is too high
# endif
# if BOARD_OSC32_IS_XTAL == true
#  define OSC32_MODE_VALUE       OSC32_MODE_XTAL
# else
#  define OSC32_MODE_VALUE       OSC32_MODE_EXTERNAL
# endif
#else
# if defined(BOARD_OSC32_HZ)
#  error BOARD_OSC32_STARTUP_US must be defined by the board code
# endif
# ifdef __DOXYGEN__
#  define OSC32_STARTUP_VALUE     UNDEFINED
#  define OSC32_STARTUP_TIMEOUT   UNDEFINED
#  define OSC32_MODE_VALUE        UNDEFINED
# endif
#endif

/**
 * \name Board-specific configuration parameters
 * The following definitions must be provided by the board code for all
 * working oscillators on the board.
 */
//@{
/**
 * \def BOARD_OSC0_HZ
 * \brief Clock frequency of OSC0 in Hz
 */
/**
 * \def BOARD_OSC0_STARTUP_US
 * \brief Startup time of OSC0 in microseconds
 */
/**
 * \def BOARD_OSC0_IS_XTAL
 * \brief OSC0 uses a crystal, not an external clock
 */
/**
 * \def BOARD_OSC32_HZ
 * \brief Clock frequency of OSC32 in Hz
 */
/**
 * \def BOARD_OSC32_STARTUP_US
 * \brief Startup time of OSC32 in microseconds
 */
/**
 * \def BOARD_OSC32_IS_XTAL
 * \brief OSC32 uses a crystal, not an external clock
 */
/**
 * \def BOARD_OSC32_PINSEL
 * \brief If set to 1, use XIN32_2/XOUT32_2 pins for OSC32
 *
 * If not defined, the primary XIN32/XOUT32 pins are used.
 */
#ifndef BOARD_OSC32_PINSEL
# define BOARD_OSC32_PINSEL     0
#endif

/**
 * \name RC oscillator frequency limits
 * The frequency of the internal RC oscillators may drift a bit as a
 * result of temperature changes. These definitions provide upper and
 * lower limits which may be used to calculate upper and lower limits of
 * timeouts, derived clock frequencies, etc.
 */
//@{
//! Nominal frequency of RCSYS in Hz
#define OSC_RCSYS_NOMINAL_HZ    115000
//! Minimum frequency of RCSYS in Hz
#define OSC_RCSYS_MIN_HZ        100000
//! Maximum frequency of RCSYS in Hz
#define OSC_RCSYS_MAX_HZ        120000
//! Nominal frequency of RC32K in Hz
#define OSC_RC32K_NOMINAL_HZ    32000
//! Minimum frequency of RC32K in Hz
#define OSC_RC32K_MIN_HZ        20000
//! Maximum frequency of RC32K in Hz
#define OSC_RC32K_MAX_HZ        44000
//! Nominal frequency of RC120M in Hz
#define OSC_RC120M_NOMINAL_HZ   120000000
//@}

#ifndef __ASSEMBLY__

#include <stdbool.h>
#include <stdint.h>
#include <avr32/io.h>

extern void osc_priv_enable_osc0(void);
extern void osc_priv_disable_osc0(void);
extern void osc_priv_enable_osc32(void);
extern void osc_priv_disable_osc32(void);
extern void osc_priv_enable_rc32k(void);
extern void osc_priv_disable_rc32k(void);
extern void osc_priv_enable_rc120m(void);
extern void osc_priv_disable_rc120m(void);

static inline void osc_enable(uint8_t id)
{
	switch (id) {
#ifdef BOARD_OSC0_HZ
	case OSC_ID_OSC0:
		osc_priv_enable_osc0();
		break;
#endif

#ifdef BOARD_OSC32_HZ
	case OSC_ID_OSC32:
		osc_priv_enable_osc32();
		break;
#endif

	case OSC_ID_RC32K:
		osc_priv_enable_rc32k();
		break;

	case OSC_ID_RC120M:
		osc_priv_enable_rc120m();
		break;

	case OSC_ID_RCSYS:
		/* RCSYS is always running */
		break;

	default:
		/* unhandled_case(id); */
		break;
	}
}

static inline void osc_disable(uint8_t id)
{
	switch (id) {
#ifdef BOARD_OSC0_HZ
	case OSC_ID_OSC0:
		osc_priv_disable_osc0();
		break;
#endif

#ifdef BOARD_OSC32_HZ
	case OSC_ID_OSC32:
		osc_priv_disable_osc32();
		break;
#endif

	case OSC_ID_RC32K:
		osc_priv_disable_rc32k();
		break;

	case OSC_ID_RC120M:
		osc_priv_disable_rc120m();
		break;

	case OSC_ID_RCSYS:
		/* RCSYS is always running */
		break;

	default:
		/* unhandled_case(id); */
		break;
	}
}

static inline bool osc_is_ready(uint8_t id)
{
	switch (id) {
#ifdef BOARD_OSC0_HZ
	case OSC_ID_OSC0:
#if (UC3L3_L4 || UC3L0128 || UC3L0256)
		return !!(AVR32_SCIF.pclksr & (1 << AVR32_SCIF_PCLKSR_OSC0RDY));
#else
		return !!(AVR32_SCIF.pclksr & (1 << AVR32_SCIF_OSC0RDY));
#endif
#endif

#ifdef BOARD_OSC32_HZ
	case OSC_ID_OSC32:
#if (UC3L3_L4 || UC3L0128 || UC3L0256)
		return !!(AVR32_SCIF.pclksr & (1 << AVR32_SCIF_PCLKSR_OSC32RDY));
#else
		return !!(AVR32_SCIF.pclksr & (1 << AVR32_SCIF_OSC32RDY));
#endif
#endif

	case OSC_ID_RC32K:
		return !!(AVR32_SCIF.rc32kcr & (1 << AVR32_SCIF_RC32KCR_EN));

	case OSC_ID_RC120M:
		return !!(AVR32_SCIF.rc120mcr & (1 << AVR32_SCIF_RC120MCR_EN));

	case OSC_ID_RCSYS:
		/* RCSYS is always ready */
		return true;

	default:
		/* unhandled_case(id); */
		return false;
	}
}

static inline uint32_t osc_get_rate(uint8_t id)
{
	switch (id) {
#ifdef BOARD_OSC0_HZ
	case OSC_ID_OSC0:
		return BOARD_OSC0_HZ;
#endif

#ifdef BOARD_OSC32_HZ
	case OSC_ID_OSC32:
		return BOARD_OSC32_HZ;
#endif

	case OSC_ID_RC32K:
		return OSC_RC32K_NOMINAL_HZ;

	case OSC_ID_RC120M:
		return OSC_RC120M_NOMINAL_HZ;

	case OSC_ID_RCSYS:
		return OSC_RCSYS_NOMINAL_HZ;

	default:
		/* unhandled_case(id); */
		return 0;
	}
}

#endif /* !__ASSEMBLY__ */

//! @}

#ifdef __cplusplus
}
#endif

#endif /* CHIP_OSC_H_INCLUDED */
