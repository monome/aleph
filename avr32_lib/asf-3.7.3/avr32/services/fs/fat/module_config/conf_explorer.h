/**************************************************************************
 *
 * \file
 *
 * \brief FAT configuration file.
 *
 * This file contains the possible external configuration of the FAT.
 *
 * Information about file-system limitations:
 *  - Only 2-FAT FSs supported by this navigator.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/


#ifndef _CONF_EXPLORER_H_
#define _CONF_EXPLORER_H_

#include "compiler.h"


//! Include files and compile options (do not change these settings).
#define LIB_MEM               <string.h>
#define LIB_CTRLACCESS        "ctrl_access.h"

//! Supported FATs (\c true or \c false).
#define FS_FAT_12             true
#define FS_FAT_16             true
#define FS_FAT_32             true

//! The explorer may support either the ASCII or the UNICODE string format, or both.
#define FS_ASCII              true
#define FS_UNICODE            false

//! The navigator may support only the first partition (\c false), or multiple partitions (\c true).
#define FS_MULTI_PARTITION    false

//! Maximal number of characters in file path.
#define MAX_FILE_PATH_LENGTH  30

//! Maximal size of configuration file.
#define MAX_CONFIG_FILE_SIZE  110

//! Level of features.
//! Select among:
//!  - \c FSFEATURE_READ:           All read functions.
//!  - \c FSFEATURE_WRITE:          nav_file_copy(), nav_file_paste(), nav_file_del(), file_create(), file_open(MODE_WRITE), file_write(), file_putc().
//!  - \c FSFEATURE_WRITE_COMPLET:  FSFEATURE_WRITE functions and nav_drive_format(), nav_dir_make(), nav_file_rename(), nav_file_dateset(), nav_file_attributset().
//!  - \c FSFEATURE_ALL:            All functions.
#define FS_LEVEL_FEATURES     (0)
#warning "FAT Level of Features set to None by default: edit the conf_explorer.h file and modify the FS_LEVEL_FEATURES define value according to the application."

//! Number of caches used to store a cluster list of files (interesting in case of many `open file').
//! In player mode, 1 is OK (shall be > 0).
#define FS_NB_CACHE_CLUSLIST  1

//! Maximal number of simultaneous navigators.
#define FS_NB_NAVIGATOR       2

//! Number of reserved navigators (ids from \c 0 to <tt>(FS_NB_RESERVED_NAVIGATOR - 1)</tt>).
#define FS_NB_RESERVED_NAV    0

/*! \name Navigator Affiliations
 *
 * Define the affiliations of the navigators.
 *
 * \note The explorer always has the navigator ID 0.
 */
//! @{

//! The explorer uses the navigator ID 1 to open the `copy file' and the ID 0 to open the `paste file'.
#define FS_NAV_ID_COPYFILE    1

//! Navigator used to open the file playlist
#define FS_NAV_ID_PLAYLIST          2

//! @}

/*! \name Navigation Automatic Configuration
 */
//! @{
// Number maximum of file include in each random (Unit 8 files)
#define  NAVAUTO_MAX_RANGE_RAND  8     // 8*8= 64 file
// Random value method (byte value)
#define NAVAUTO_GET_RAND(val)       (val=1) 	// By default no random value but static value

#define NAV_AUTO_FILE_IN_FIRST   1

//! Enable speed optimization
#define NAVAUTO_SPEED_OPTIMIZATION              false
//! It defines the number of level in the tree architecture that will be impacted by this optimization.\n
//! If this number is equal to 1, then only the 1st folder level will be impacted by this optimization.\n
//! In counter part, the higher this number is, the more RAM memory it will need.
#define NAVAUTO_SPEED_OPTIMIZATION_LEVEL        8
//! @}

/*! \name Playlist Configuration
 */
//! @{

//! Cache to store the path of a file (here, mapped onto a global buffer).
#define pl_cache_path           g_buffer

//! Size of cache to store the path of a file.
#define PL_CACHE_PATH_MAX_SIZE  sizeof(g_buffer)

//! Playlist format: \c false for ASCII or \c true for UNICODE.
#define PL_UNICODE              false

//! @}


#endif  // _CONF_EXPLORER_H_
