/**
 * \file
 *
 * \brief Chip-specific system clock management functions
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
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
#include <avr32/io.h>
#include <compiler.h>
#include <stdbool.h>
#include <sysclk.h>
#include <flashc.h>

/**
 * \weakgroup sysclk_group
 * @{
 */

/**
 * \internal
 * \defgroup sysclk_internals_group System Clock internals
 *
 * System clock management is fairly straightforward apart from one
 * thing: Enabling and disabling bus bridges. When all peripherals on a
 * given bus are disabled, the bridge to the bus may be disabled. Only
 * the PBA, PBB and PBC busses support this, and it is not practical to
 * disable the PBA bridge as it includes the Power Manager, so turning
 * it off would make it impossible to turn anything back on again.
 *
 * @{
 */

#if ((CONFIG_SYSCLK_CPU_DIV > CONFIG_SYSCLK_PBA_DIV) || \
     (CONFIG_SYSCLK_CPU_DIV > CONFIG_SYSCLK_PBB_DIV) || \
     (CONFIG_SYSCLK_CPU_DIV > CONFIG_SYSCLK_PBC_DIV))
# error CONFIG_SYSCLK_PBx_DIV must be equal to or more than CONFIG_SYSCLK_CPU_DIV.
#endif

/**
 * \internal
 * \name Initial module clock masks
 *
 * These are the mask values written to the xxxMASK registers during
 * initialization if the user has overridden the default behavior of all clocks
 * left enabled. These values assume that:
 *   - Debugging should be possible
 *   - The program may be running from flash
 *   - The PM should be available to unmask other clocks
 *   - All on-chip RAM should be available
 *   - INTC, SCIF, PM and GPIO are made permanently available for now; this
 *     may change in the future.
 */
//@{
//! \internal
//! \brief Initial value of CPUMASK
#define SYSCLK_INIT_MINIMAL_CPUMASK                                    \
	((1 << SYSCLK_OCD)                                                 \
		| (1 << SYSCLK_SYSTIMER))
//! \internal
//! \brief Initial value of HSBMASK
#define SYSCLK_INIT_MINIMAL_HSBMASK                                    \
	((1 << SYSCLK_FLASHC_DATA)                                         \
		| (1 << SYSCLK_PBA_BRIDGE)                                     \
		| (1 << SYSCLK_HSB_RAM))
//! \internal
//! \brief Initial value of PBAMASK
#define SYSCLK_INIT_MINIMAL_PBAMASK                                    \
	((1 << SYSCLK_INTC)                                                \
		| (1 << SYSCLK_GPIO)                                           \
		| (1 << SYSCLK_SCIF)                                           \
		| (1 << SYSCLK_PM))
//! \internal
//! \brief Initial value of PBBMASK
#define SYSCLK_INIT_MINIMAL_PBBMASK     0
//! \internal
//! \brief Initial value of PBCMASK
#define SYSCLK_INIT_MINIMAL_PBCMASK     0
//@}

#if defined(CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
/**
 * \brief boolean signalling that the sysclk_init is done.
 */
bool sysclk_initialized = false;
#endif

/**
 * \internal
 * \brief Enable a maskable module clock.
 * \param bus_id Bus index, given by the \c AVR32_PM_CLK_GRP_xxx definitions.
 * \param module_index Index of the module to be enabled. This is the
 * bit number in the corresponding xxxMASK register.
 */
void sysclk_priv_enable_module(unsigned int bus_id, unsigned int module_index)
{
	irqflags_t flags;
	uint32_t   mask;

	flags = cpu_irq_save();

	/* Enable the clock */
	mask = *(&AVR32_PM.cpumask + bus_id);
	mask |= 1U << module_index;
	AVR32_PM.unlock = 0xaa000020 + (4 * bus_id);
	*(&AVR32_PM.cpumask + bus_id) = mask;

	cpu_irq_restore(flags);
}

/**
 * \internal
 * \brief Disable a maskable module clock.
 * \param bus_id Bus index, given by the \c AVR32_PM_CLK_GRP_xxx definitions.
 * \param module_index Index of the module to be disabled. This is the
 * bit number in the corresponding xxxMASK register.
 */
void sysclk_priv_disable_module(unsigned int bus_id, unsigned int module_index)
{
	irqflags_t flags;
	uint32_t   mask;

	flags = cpu_irq_save();

	/* Disable the clock */
	mask = *(&AVR32_PM.cpumask + bus_id);
	mask &= ~(1U << module_index);
	AVR32_PM.unlock = 0xaa000020 + (4 * bus_id);
	*(&AVR32_PM.cpumask + bus_id) = mask;

	cpu_irq_restore(flags);
}

//! @}

/**
 * \brief Set system clock prescaler configuration
 *
 * This function will change the system clock prescaler configuration to
 * match the parameters.
 *
 * \note The parameters to this function are device-specific.
 *
 * \param cpu_shift The CPU clock will be divided by \f$2^{cpu\_shift}\f$
 * \param pba_shift The PBA clock will be divided by \f$2^{pba\_shift}\f$
 * \param pbb_shift The PBB clock will be divided by \f$2^{pbb\_shift}\f$
 * \param pbc_shift The PBC clock will be divided by \f$2^{pbc\_shift}\f$
 */
void sysclk_set_prescalers(unsigned int cpu_shift,
		unsigned int pba_shift, unsigned int pbb_shift,
		unsigned int pbc_shift)
{
	irqflags_t flags;
	uint32_t   cpu_cksel = 0;
	uint32_t   pba_cksel = 0;
	uint32_t   pbb_cksel = 0;
	uint32_t   pbc_cksel = 0;

	Assert(cpu_shift <= pba_shift);
	Assert(cpu_shift <= pbb_shift);

	if (cpu_shift > 0)
		cpu_cksel = ((cpu_shift - 1) << AVR32_PM_CPUSEL_CPUSEL)
				| (1U << AVR32_PM_CPUDIV);

	if (pba_shift > 0)
		pba_cksel = ((pba_shift - 1) << AVR32_PM_PBASEL_PBSEL)
				| (1U << AVR32_PM_PBADIV);

	if (pbb_shift > 0)
		pbb_cksel = ((pbb_shift - 1) << AVR32_PM_PBBSEL_PBSEL)
				| (1U << AVR32_PM_PBBDIV);

	if (pbc_shift > 0)
		pbc_cksel = ((pbc_shift - 1) << AVR32_PM_PBCSEL_PBSEL)
				| (1U << AVR32_PM_PBCDIV);

	flags = cpu_irq_save();
	AVR32_PM.unlock = 0xaa000000 | AVR32_PM_CPUSEL;
	AVR32_PM.cpusel = cpu_cksel;
	AVR32_PM.unlock = 0xaa000000 | AVR32_PM_PBASEL;
	AVR32_PM.pbasel = pba_cksel;
	AVR32_PM.unlock = 0xaa000000 | AVR32_PM_PBBSEL;
	AVR32_PM.pbbsel = pbb_cksel;
	AVR32_PM.unlock = 0xaa000000 | AVR32_PM_PBCSEL;
	AVR32_PM.pbcsel = pbc_cksel;
	cpu_irq_restore(flags);
}

/**
 * \brief Change the source of the main system clock.
 *
 * \param src The new system clock source. Must be one of the constants
 * from the <em>System Clock Sources</em> section.
 */
void sysclk_set_source(uint_fast8_t src)
{
	irqflags_t flags;

	Assert(src <= SYSCLK_SRC_RC120M);

	flags = cpu_irq_save();
	AVR32_PM.unlock = 0xaa000000 | AVR32_PM_MCCTRL;
	AVR32_PM.mcctrl = src;
	cpu_irq_restore(flags);
}

#if defined(CONFIG_USBCLK_SOURCE) || defined(__DOXYGEN__)
/**
 * \brief Enable the USB generic clock
 *
 * \pre The USB generic clock must be configured to 48MHz.
 * CONFIG_USBCLK_SOURCE and CONFIG_USBCLK_DIV must be defined with proper
 * configuration. The selected clock source must also be configured.
 */
void sysclk_enable_usb(void)
{
	genclk_enable_config(AVR32_USBC_GCLK_NUM, CONFIG_USBCLK_SOURCE, CONFIG_USBCLK_DIV);
}

/**
 * \brief Disable the USB generic clock
 */
void sysclk_disable_usb(void)
{
	genclk_disable(AVR32_USBC_GCLK_NUM);
}
#endif // CONFIG_USBCLK_SOURCE


void sysclk_init(void)
{
	/* Set up system clock dividers if different from defaults */
	if ((CONFIG_SYSCLK_CPU_DIV > 0) || (CONFIG_SYSCLK_PBA_DIV > 0) ||
			(CONFIG_SYSCLK_PBB_DIV > 0) || (CONFIG_SYSCLK_PBC_DIV > 0)) {
		sysclk_set_prescalers(CONFIG_SYSCLK_CPU_DIV,
				CONFIG_SYSCLK_PBA_DIV,
				CONFIG_SYSCLK_PBB_DIV,
				CONFIG_SYSCLK_PBC_DIV);
	}

	/* Switch to system clock selected by user */
	switch (CONFIG_SYSCLK_SOURCE) {
	case SYSCLK_SRC_RCSYS:
		/* Already running from RCSYS */
		break;

#ifdef BOARD_OSC0_HZ
	case SYSCLK_SRC_OSC0:
		osc_enable(OSC_ID_OSC0);
		osc_wait_ready(OSC_ID_OSC0);
		// Set a flash wait state depending on the new cpu frequency.
		flash_set_bus_freq(BOARD_OSC0_HZ);
		sysclk_set_source(SYSCLK_SRC_OSC0);
		break;
#endif

#ifdef BOARD_OSC1_HZ
	case SYSCLK_SRC_OSC1:
		osc_enable(OSC_ID_OSC1);
		osc_wait_ready(OSC_ID_OSC1);
		// Set a flash wait state depending on the new cpu frequency.
		flash_set_bus_freq(BOARD_OSC1_HZ);
		sysclk_set_source(SYSCLK_SRC_OSC1);
		break;
#endif

#ifdef CONFIG_PLL0_SOURCE
	case SYSCLK_SRC_PLL0: {
		pll_enable_config_defaults(0);
		// Set a flash wait state depending on the new cpu frequency.
		flash_set_bus_freq(sysclk_get_cpu_hz());
		sysclk_set_source(SYSCLK_SRC_PLL0);
		break;
	}
#endif

#ifdef CONFIG_PLL1_SOURCE
	case SYSCLK_SRC_PLL1: {
		pll_enable_config_defaults(1);
		// Set a flash wait state depending on the new cpu frequency.
		flash_set_bus_freq(sysclk_get_cpu_hz());
		sysclk_set_source(SYSCLK_SRC_PLL1);
		break;
	}
#endif

	case SYSCLK_SRC_RC8M:
		osc_enable(OSC_ID_RC8M);
		osc_wait_ready(OSC_ID_RC8M);
		sysclk_set_source(SYSCLK_SRC_RC8M);
		break;
		
	case SYSCLK_SRC_RC120M:
		osc_enable(OSC_ID_RC120M);
		osc_wait_ready(OSC_ID_RC120M);
		// Set a flash wait state depending on the new cpu frequency.
		flash_set_bus_freq(sysclk_get_cpu_hz());
		sysclk_set_source(SYSCLK_SRC_RC120M);
		break;		

	default:
		Assert(false);
		break;
	}

	/* If the user has specified clock masks, enable only requested clocks */
#if defined(CONFIG_SYSCLK_INIT_CPUMASK)
	AVR32_PM.cpumask = SYSCLK_INIT_MINIMAL_CPUMASK | CONFIG_SYSCLK_INIT_CPUMASK;
#endif
#if defined(CONFIG_SYSCLK_INIT_PBAMASK)
	AVR32_PM.pbamask = SYSCLK_INIT_MINIMAL_PBAMASK | CONFIG_SYSCLK_INIT_PBAMASK;
#endif
#if defined(CONFIG_SYSCLK_INIT_PBBMASK)
	AVR32_PM.pbbmask = SYSCLK_INIT_MINIMAL_PBBMASK | CONFIG_SYSCLK_INIT_PBBMASK;
#endif
#if defined(CONFIG_SYSCLK_INIT_PBCMASK)
	AVR32_PM.pbcmask = SYSCLK_INIT_MINIMAL_PBCMASK | CONFIG_SYSCLK_INIT_PBCMASK;
#endif
#if defined(CONFIG_SYSCLK_INIT_HSBMASK)
	AVR32_PM.hsbmask = SYSCLK_INIT_MINIMAL_HSBMASK | CONFIG_SYSCLK_INIT_HSBMASK;
#endif

#if (defined CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
	/* Signal that the internal frequencies are setup */
	sysclk_initialized = true;
#endif
}

//! @}
