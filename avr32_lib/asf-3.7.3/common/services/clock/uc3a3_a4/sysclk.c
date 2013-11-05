/**
 * \file
 *
 * \brief Chip-specific system clock management functions
 *
 * Copyright (c) 2009-2011 Atmel Corporation. All rights reserved.
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
#include <compiler.h>
#include <stdbool.h>
#include <sysclk.h>
#include <avr32/io.h>
#include <flashc.h>


/**
 * \weakgroup sysclk_group
 * @{
 */

#if ((CONFIG_SYSCLK_CPU_DIV > CONFIG_SYSCLK_PBA_DIV) || \
     (CONFIG_SYSCLK_CPU_DIV > CONFIG_SYSCLK_PBB_DIV))
# error CONFIG_SYSCLK_PBx_DIV must be equal to or more than CONFIG_SYSCLK_CPU_DIV.
#endif

/**
 * \internal
 * \defgroup sysclk_internals_group System Clock internals
 *
 * System clock management is fairly straightforward apart from one
 * thing: Enabling and disabling bus bridges. When all peripherals on a
 * given bus are disabled, the bridge to the bus may be disabled. Only
 * the PBA, PBB and PBC busses support this, and it is not practical to
 * disable the PBA bridge as it includes the Power Manager, so turning
 * it off would make it impossible to turn anything back on again. Also,
 * PBB and PBC share the same MASK register (with the AES module being
 * the only peripheral on the PBC.)
 *
 * The system clock implementation keeps track of a reference count for
 * PBB and PBC. When the reference count is zero, the bus bridge is
 * disabled, otherwise it is enabled.
 *
 * @{
 */

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
 *   - INTC, PM and GPIO are made permanently available for now; this
 *     may change in the future.
 */
//@{
//! \internal
//! \brief Initial value of CPUMASK
#define SYSCLK_INIT_MINIMAL_CPUMASK                                   \
	((1 << SYSCLK_OCD)                                                \
		| (1 << SYSCLK_SYSTIMER))

//! \internal
//! \brief Sticky Bits in HSBMASK according to the datasheet
#define SYSCLK_HSBMASK_STICKYBITS (0x00000800)
//! \internal
//! \brief Initial value of HSBMASK
#define SYSCLK_INIT_MINIMAL_HSBMASK                                    \
	((1 << SYSCLK_FLASHC_DATA)                                         \
		| (1 << SYSCLK_PBA_BRIDGE)                                     \
		| (1 << SYSCLK_HRAMC0)                                         \
		| (1 << SYSCLK_HRAMC1)                                         \
		| (SYSCLK_HSBMASK_STICKYBITS))

//! \internal
//! \brief Sticky Bits in PBAMASK according to the datasheet
#define SYSCLK_PBAMASK_STICKYBITS (0x00180000)
//! \internal
//! \brief Initial value of PBAMASK
#define SYSCLK_INIT_MINIMAL_PBAMASK                                    \
	((1 << SYSCLK_INTC)                                                \
		| (1 << SYSCLK_GPIO)                                           \
		| (1 << SYSCLK_PM)                                             \
		| (SYSCLK_PBAMASK_STICKYBITS))

//! \internal
//! \brief Initial value of PBBMASK
#define SYSCLK_INIT_MINIMAL_PBBMASK     0
//@}

/**
 * \internal
 * \brief Number of enabled peripherals on the PBB and PBC busses.
 */
static uint8_t sysclk_bus_refcount[2];

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

	/*
	 * Poll MSKRDY before changing mask rather than after, as it's
	 * highly unlikely to actually be cleared at this point.
	 */
	while (!(AVR32_PM.poscsr & (1U << AVR32_PM_POSCSR_MSKRDY))) {
		/* Do nothing */
	}

	/* Enable the clock */
	mask = *(&AVR32_PM.cpumask + bus_id);
	mask |= 1U << module_index;
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

	/*
	 * Poll MSKRDY before changing mask rather than after, as it's
	 * highly unlikely to actually be cleared at this point.
	 */
	while (!(AVR32_PM.poscsr & (1U << AVR32_PM_POSCSR_MSKRDY))) {
		/* Do nothing */
	}

	/* Disable the clock */
	mask = *(&AVR32_PM.cpumask + bus_id);
	mask &= ~(1U << module_index);
	*(&AVR32_PM.cpumask + bus_id) = mask;

	cpu_irq_restore(flags);
}

//! @}

/**
 * \brief Enable a module clock derived from the PBB clock
 * \param index Index of the module clock in the PBBMASK register
 */
void sysclk_enable_pbb_module(unsigned int index)
{
	unsigned int pbus_id = 0;
	irqflags_t   flags;

	/* The AES module is on PBC, others are on PBB */
	if (index == SYSCLK_AES)
		pbus_id = 1;

	/* Enable the bridge if necessary */
	flags = cpu_irq_save();

	if (!sysclk_bus_refcount[pbus_id])
		sysclk_enable_hsb_module(2 + (4 * pbus_id));
	sysclk_bus_refcount[pbus_id]++;

	cpu_irq_restore(flags);

	/* Enable the module */
	sysclk_priv_enable_module(AVR32_PM_CLK_GRP_PBB, index);
}

/**
 * \brief Disable a module clock derived from the PBB clock
 * \param index Index of the module clock in the PBBMASK register
 */
void sysclk_disable_pbb_module(unsigned int index)
{
	unsigned int pbus_id = 0;
	irqflags_t   flags;

	/* Disable the module */
	sysclk_priv_disable_module(AVR32_PM_CLK_GRP_PBB, index);

	/* The AES module is on PBC, others are on PBB */
	if (index == SYSCLK_AES)
		pbus_id = 1;

	/* Disable the bridge if possible */
	flags = cpu_irq_save();

	sysclk_bus_refcount[pbus_id]--;
	if (!sysclk_bus_refcount[pbus_id])
		sysclk_disable_hsb_module(2 + (4 * pbus_id));

	cpu_irq_restore(flags);
}

#if defined(CONFIG_USBCLK_SOURCE) || defined(__DOXYGEN__)
/**
 * \brief Enable the USB generic clock
 *
 * \pre The USB generic clock must be configured to 12MHz.
 * CONFIG_USBCLK_SOURCE and CONFIG_USBCLK_DIV must be defined with proper
 * configuration. The selected clock source must also be configured.
 */
void sysclk_enable_usb(void)
{
	sysclk_enable_pbb_module(SYSCLK_USBB_REGS);
	sysclk_enable_hsb_module(SYSCLK_USBB_DATA);
	genclk_enable_config(AVR32_PM_GCLK_USBB, CONFIG_USBCLK_SOURCE, CONFIG_USBCLK_DIV);
}

/**
 * \brief Disable the USB generic clock
 */
void sysclk_disable_usb(void)
{
	genclk_disable(AVR32_PM_GCLK_USBB);
	sysclk_disable_hsb_module(SYSCLK_USBB_DATA);
	sysclk_disable_pbb_module(SYSCLK_USBB_REGS);
}
#endif // CONFIG_USBCLK_SOURCE


void sysclk_init(void)
{
	/* Set up system clock dividers if different from defaults */
	if ((CONFIG_SYSCLK_CPU_DIV > 0) || (CONFIG_SYSCLK_PBA_DIV > 0) ||
			(CONFIG_SYSCLK_PBB_DIV > 0)) {
		sysclk_set_prescalers(CONFIG_SYSCLK_CPU_DIV,
				CONFIG_SYSCLK_PBA_DIV,
				CONFIG_SYSCLK_PBB_DIV);
	}

	/* Switch to system clock selected by user */
	switch (CONFIG_SYSCLK_SOURCE) {
	case SYSCLK_SRC_RCSYS:
		/* Already running from RCOSC */
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

#ifdef CONFIG_PLL0_SOURCE
	case SYSCLK_SRC_PLL0: {
		pll_enable_config_defaults(0);
		// Set a flash wait state depending on the new cpu frequency.
		flash_set_bus_freq(sysclk_get_cpu_hz());
		sysclk_set_source(SYSCLK_SRC_PLL0);
		break;
	}
#endif

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
#if defined(CONFIG_SYSCLK_INIT_HSBMASK)
	AVR32_PM.hsbmask = SYSCLK_INIT_MINIMAL_HSBMASK | CONFIG_SYSCLK_INIT_HSBMASK;
#endif

#if (defined CONFIG_SYSCLK_DEFAULT_RETURNS_SLOW_OSC)
	/* Signal that the internal frequencies are setup */
	sysclk_initialized = true;
#endif
}

//! @}
