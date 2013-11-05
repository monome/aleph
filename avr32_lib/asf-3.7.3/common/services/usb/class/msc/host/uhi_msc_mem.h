/**
 * \file
 *
 * \brief USB host Mass Storage interface for control access module.
 *
 * Copyright (C) 2011-2012 Atmel Corporation. All rights reserved.
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

#ifndef _UHI_MSC_MEM_H_
#define _UHI_MSC_MEM_H_

#include "ctrl_access.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup uhi_msc_group
 * \defgroup uhi_msc_mem_group USB host Mass Storage interface for control access module
 * Layer added on UHI MSC interface to allow the usage of control access module.
 * The control access module provides a common access at all memories and
 * it is used by the File Systems available in ASF.
 *
 * See \ref uhi_msc_mem_quickstart.
 * @{
 */

/**
 * \brief Gives the number of available LUN
 *
 * Note: A LUN can be available, but with a status not present.
 * It is the case for a card reader without card.
 *
 * \return Number of available LUN
 */
uint8_t uhi_msc_mem_get_lun(void);

/**
 * \brief Checks and update the status of the LUN
 *
 * \param lun       LUN number
 *
 * \return Status of the LUN
 */
Ctrl_status uhi_msc_mem_test_unit_ready(uint8_t lun);

/**
 * \brief Returns the capacity of the LUN
 *
 * \param lun           LUN number
 * \param u32_nb_sector Pointer to store the last sector address possible on this LUN
 *
 * \return Status of the LUN
 */
Ctrl_status uhi_msc_mem_read_capacity(uint8_t lun, uint32_t *u32_nb_sector);

/**
 * \brief Returns the sector size of the LUN
 *
 * \param lun           LUN number
 *
 * \return Sector size (unit 512B)
 */
uint8_t uhi_msc_mem_read_sector_size(uint8_t lun);

/**
 * \brief Checks if the LUN is write protected
 *
 * \param lun           LUN number
 *
 * \return true, if write protected
 */
bool uhi_msc_mem_wr_protect(uint8_t lun);

/**
 * \brief Checks if the device is removed
 *
 * \return Always true for USB Device
 */
bool uhi_msc_mem_removal(void);

/**
 * \brief Reads 512 bytes from the current LUN
 *
 * The LUN is selected by uhi_msc_mem_test_unit_ready()
 * or uhi_msc_mem_read_capacity() function.
 *
 * \param addr  Disk address (unit 512B)
 * \param ram   Pointer to store the data
 *
 * \return Status of the LUN
 */
Ctrl_status uhi_msc_mem_read_10_ram(uint32_t addr, void *ram);

/**
 * \brief Writes 512 bytes to the current LUN
 *
 * The LUN is selected by uhi_msc_mem_test_unit_ready()
 * or uhi_msc_mem_read_capacity() function.
 *
 * \param addr  Disk address (unit 512B)
 * \param ram   Pointer on the data
 *
 * \return Status of the LUN
 */
Ctrl_status uhi_msc_mem_write_10_ram(uint32_t addr, const void *ram);

//@}

/**
 * \page uhi_msc_mem_quickstart Quick start guide for USB host mass-storage module (UHI MSC)
 *
 * This is the quick start guide for the \ref uhi_msc_mem_group 
 * "USB host mass-storage module (UHI MSC)" with step-by-step instructions on 
 * how to configure and use the modules in a selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 * 
 * \section uhi_msc_mem_basic_use_case Basic use case
 * In this basic use case, the "USB Host MSC (Single Class support)" module is used.
 *
 * The "USB Host MSC (Multiple Classes support)" module usage is described
 * in \ref uhi_msc_mem_use_cases "Advanced use cases".
 *
 * This example do a simple physical memory access, but a File System module
 * can be added to decode the USB memory file system, see FatFS examples. 
 *
 * \section uhi_msc_mem_basic_use_case_setup Setup steps
 * \subsection uhi_msc_mem_basic_use_case_setup_prereq Prerequisites
 * \copydetails uhc_basic_use_case_setup_prereq
 * \subsection uhi_msc_mem_basic_use_case_setup_code Example code
 * \copydetails uhc_basic_use_case_setup_code
 * \subsection uhi_msc_mem_basic_use_case_setup_flow Workflow
 * \copydetails uhc_basic_use_case_setup_flow
 *
 * \section uhi_msc_mem_basic_use_case_usage Usage steps
 *
 * \subsection uhi_msc_mem_basic_use_case_usage_code Example code
 * Content of conf_usb_host.h:
 * \code
 * #define USB_HOST_UHI        UHI_MSC
 * #define UHI_MSC_CHANGE(dev, b_plug) my_callback_msc_change(dev, b_plug)
 * extern bool my_callback_msc_change(uhc_device_t* dev, bool b_plug);
 * #include "uhi_msc_mem.h" // At the end of conf_usb_host.h file
 * \endcode
 *
 * Add to application C-file:
 * \code
 * static bool my_flag_autorize_msc_check = false;
 * bool my_callback_msc_change(uhc_device_t* dev, bool b_plug)
 * {
 *    if (b_plug) {
 *       my_flag_autorize_msc_check = true;
 *    } else {
 *       my_flag_autorize_msc_check = false;
 *    }
 * }
 *
 * void my_task(void)
 * {
 *    if (!my_flag_autorize_msc_check) {
 *       return;
 *    }
 *    my_flag_autorize_msc_check = false;
 *
 *    // Check all new USB disks plugged
 *    for (uint8_t lun=0; lun < uhi_msc_mem_get_lun(); lun++) {
 *       // Wait the end of USB disk install
 *       while (CTRL_BUSY == uhi_msc_mem_test_unit_ready(lun));
 *       if (CTRL_GOOD != uhi_msc_mem_test_unit_ready(lun)) {
 *          // Removal disk not present or fail
 *          continue;
 *       }
 *       // Read capacity
 *       uint32_t max_lba;
 *       uhi_msc_mem_read_capacity(lun, &max_lba);
 *    }
 * }
 * \endcode
 *
 * \subsection uhi_msc_mem_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_usb_host.h is available and contains the following configuration
 * which is the USB host MSC configuration:
 *   - \code #define USB_HOST_UHI   UHI_MSC \endcode
 *     \note It defines the list of UHI supported by USB host.
 *   - \code #define UHI_MSC_CHANGE(dev, b_plug) my_callback_msc_change(dev, b_plug)
 * extern bool my_callback_msc_change(uhc_device_t* dev, bool b_plug); \endcode
 *     \note This callback is called when a USB device MSC is plugged or unplugged.
 * -# The access of the USB memories is allowed through functions described in \ref uhi_msc_mem_group.
 *
 * \section uhi_msc_mem_use_cases Advanced use cases
 * For more advanced use of the UHI MSC module, see the following use cases:
 * - \subpage uhc_use_case_1
 * - \subpage uhc_use_case_2
 * - \subpage uhc_use_case_3
 */


#ifdef __cplusplus
}
#endif

#endif // _UHI_MSC_MEM_H_
