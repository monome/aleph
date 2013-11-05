/**
 * \file
 *
 * \brief Memory access control configuration file.
 *
 * Copyright (c) 2012 - 2013 Atmel Corporation. All rights reserved.
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

#ifndef _CONF_ACCESS_H_
#define _CONF_ACCESS_H_

#include "compiler.h"
#include "board.h"


/*! \name Activation of Logical Unit Numbers
 */
/** @{ */
#define LUN_0                DISABLE    /**< On-Chip Virtual Memory. */
#define LUN_1                DISABLE    /**< AT45DBX Data Flash. */
#define LUN_2                ENABLE     /**< SD/MMC Card over HSMCI Slot 0 */
#define LUN_3                DISABLE    /**< Spare */
#define LUN_4                ENABLE     /**< NAND */
#define LUN_5                DISABLE    /**< Spare */
#define LUN_6                DISABLE    /**< Spare */
#define LUN_7                DISABLE    /**< Spare */
#define LUN_USB              DISABLE    /**< Host Mass-Storage Memory. */
/** @} */

/*! \name LUN 0 Definitions
 */
/** @{ */
#define VIRTUAL_MEM                             LUN_0
#define LUN_ID_VIRTUAL_MEM                      LUN_ID_0
#define LUN_0_INCLUDE                           "virtual_mem.h"
#define Lun_0_test_unit_ready                   virtual_test_unit_ready
#define Lun_0_read_capacity                     virtual_read_capacity
#define Lun_0_wr_protect                        virtual_wr_protect
#define Lun_0_removal                           virtual_removal
#define Lun_0_usb_read_10                       virtual_usb_read_10
#define Lun_0_usb_write_10                      virtual_usb_write_10
#define Lun_0_mem_2_ram                         virtual_mem_2_ram
#define Lun_0_ram_2_mem                         virtual_ram_2_mem
#define LUN_0_NAME                              "\"On-Chip Virtual Memory\""
/** @} */

/*! \name LUN 1 Definitions
 */
/** @{ */
#define AT45DBX_MEM                             LUN_1
#define LUN_ID_AT45DBX_MEM                      LUN_ID_1
#define LUN_1_INCLUDE                           "at45dbx_mem.h"
#define Lun_1_test_unit_ready                   at45dbx_test_unit_ready
#define Lun_1_read_capacity                     at45dbx_read_capacity
#define Lun_1_wr_protect                        at45dbx_wr_protect
#define Lun_1_removal                           at45dbx_removal
#define Lun_1_usb_read_10                       at45dbx_usb_read_10
#define Lun_1_usb_write_10                      at45dbx_usb_write_10
#define Lun_1_mem_2_ram                         at45dbx_df_2_ram
#define Lun_1_ram_2_mem                         at45dbx_ram_2_df
#define LUN_1_NAME                              "\"AT45DBX Data Flash\""
/** @} */

/*! \name LUN 2 Definitions
 */
/** @{ */
#define SD_MMC_0_MEM                            LUN_2
#define LUN_ID_SD_MMC_0_MEM                     LUN_ID_2
#define LUN_2_INCLUDE                           "sd_mmc_mem.h"
#define Lun_2_test_unit_ready                   sd_mmc_test_unit_ready_0
#define Lun_2_read_capacity                     sd_mmc_read_capacity_0
#define Lun_2_unload                            sd_mmc_unload_0
#define Lun_2_wr_protect                        sd_mmc_wr_protect_0
#define Lun_2_removal                           sd_mmc_removal_0
#define Lun_2_usb_read_10                       sd_mmc_usb_read_10_0
#define Lun_2_usb_write_10                      sd_mmc_usb_write_10_0
#define Lun_2_mem_2_ram                         sd_mmc_mem_2_ram_0
#define Lun_2_ram_2_mem                         sd_mmc_ram_2_mem_0
#define LUN_2_NAME                              "\"SD/MMC Card Slot 0\""
/** @} */

/*! \name LUN 4 Definitions
 */
/** @{ */
#define NAND_FLASH_MEM                        	LUN_4
#define LUN_ID_NAND_FLASH_MEM                 	LUN_ID_4
#define LUN_4_INCLUDE                           "nand_flash_mem.h"
#define Lun_4_test_unit_ready                   nand_flash_test_unit_ready
#define Lun_4_read_capacity                     nand_flash_read_capacity
#define Lun_4_wr_protect                        nand_flash_wr_protect
#define Lun_4_removal                           nand_flash_removal
#define Lun_4_usb_read_10                       nand_flash_usb_read_10
#define Lun_4_usb_write_10                      nand_flash_usb_write_10
#define Lun_4_mem_2_ram                         nand_flash_mem_2_ram
#define Lun_4_ram_2_mem                         nand_flash_ram_2_mem
#define LUN_4_NAME                              "\"nand_flash on EBI\""
/** @} */

/*! \name USB LUNs Definitions
 */
/** @{ */
#define MEM_USB                                 LUN_USB
#define LUN_ID_MEM_USB                          LUN_ID_USB
#define LUN_USB_INCLUDE                         "host_mem.h"
#define Lun_usb_test_unit_ready(lun)            host_test_unit_ready(lun)
#define Lun_usb_read_capacity(lun, nb_sect)     host_read_capacity(lun, nb_sect)
#define Lun_usb_read_sector_size(lun)           host_read_sector_size(lun)
#define Lun_usb_wr_protect(lun)                 host_wr_protect(lun)
#define Lun_usb_removal()                       host_removal()
#define Lun_usb_mem_2_ram(addr, ram)            host_read_10_ram(addr, ram)
#define Lun_usb_ram_2_mem(addr, ram)            host_write_10_ram(addr, ram)
#define LUN_USB_NAME                            "\"Host Mass-Storage Memory\""
/** @} */

/*! \name Actions Associated with Memory Accesses
 *
 * Write here the action to associate with each memory access.
 *
 * \warning Be careful not to waste time in order not to disturb the functions.
 */
/** @{ */
#define memory_start_read_action(nb_sectors)    ui_start_read()
#define memory_stop_read_action()               ui_stop_read()
#define memory_start_write_action(nb_sectors)   ui_start_write()
#define memory_stop_write_action()              ui_stop_write()
#include "ui.h"
/** @} */

/*! \name Activation of Interface Features
 */
/** @{ */
#define ACCESS_USB           true    /**< MEM <-> USB interface. */
#define ACCESS_MEM_TO_RAM    false   /**< MEM <-> RAM interface. */
#define ACCESS_STREAM        false   /**< Streaming MEM <-> MEM interface. */
#define ACCESS_STREAM_RECORD false   /**< Streaming MEM <-> MEM interface in record mode. */
#define ACCESS_MEM_TO_MEM    false   /**< MEM <-> MEM interface. */
#define ACCESS_CODEC         false   /**< Codec interface. */
/** @} */

/*! \name Specific Options for Access Control
 */
/** @{ */
#define GLOBAL_WR_PROTECT    false   /**< Management of a global write protection. */
/** @} */


#endif /* _CONF_ACCESS_H_ */
