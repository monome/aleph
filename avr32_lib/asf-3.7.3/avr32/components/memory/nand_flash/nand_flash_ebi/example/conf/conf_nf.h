/*****************************************************************************
 *
 * \file
 *
 * \brief Nand Flash configuration.
 *
 * This file contains the possible external configuration of the Nand Flash
 *  interface.
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
 ******************************************************************************/



#ifndef _CONF_NF_H_
#define _CONF_NF_H_

//_____ D E F I N I T I O N ________________________________________________

// !!!!!! WARNING !!!!!!!!
// !! The auto-detect mode (#define NF_AUTO_...) may be less speed that static compilation (#define NF_TYPE_...)

//! ************ Auto-detect mode
//!
//! Values: true : Firmware will autodetect
//!         false: Firmware will not autodetect
//! Note: the 2KB and 512B detect can't be enable in same time
//!
#define NF_AUTO_DETECT_2KB      false
#define NF_AUTO_DETECT_512B     false

//! ************ Static mode
//!  To enable a reference, you shall define a NF_TYPE_X (X = reference)
#define NF_TYPE_MT29F2G08AACWP

//! ************ For all mode
//! Define the number of NandFlash connected (to be right,
//! the number of Chip Select)
#define NF_N_DEVICES            2


// ******** Examples **********
//
// One MT29F2G08AACWP connected
// #define NF_TYPE_MT29F2G08AACWP
// #define NF_N_DEVICES    1
//
// Two MT29F2G08AACWP connected
// #define NF_TYPE_MT29F2G08AACWP
// #define NF_N_DEVICES    2
//
// One MT29F16G08FAAWC connected.
// #define NF_TYPE_MT29F16G08FAAWC
// #define NF_N_DEVICES    2  // This part uses 2 Chip Selects
//
// Two MT29F16G08FAAWC connected
// #define NF_TYPE_MT29F16G08FAAWC
// #define NF_N_DEVICES    4  // 2 Chip Selects x 2 NandFlash
//

// List of supported devices
//
//    ---- T O S H I B A ----
//    NF_TYPE_TC58DVG02A1FT00    128MB (512B) 3.3     (Not tested)
//    NF_TYPE_TC58NVG0S3AFT05    128MB ( 2kB) 3.3     generic driver no supported (Not tested)
//    NF_TYPE_TH58NVG1S3AFT05    256MB ( 2kB) 3.3     generic driver no supported (Not tested)
//
//    ---- S A M S U N G ----
//    NF_TYPE_K9F1208U0M          64MB (512B) 3.3
//    NF_TYPE_K9F1G08U0M         128MB ( 2kB) 3.3
//    NF_TYPE_K9F2G08U0M         256MB ( 2kB) 3.3
//    NF_TYPE_K9F2G08U0A         256MB ( 2kB) 3.3     generic driver no supported
//    NF_TYPE_K9F2G08R0A         256MB ( 2kB) 1.8     generic driver no supported (Not tested)
//    NF_TYPE_K9F4G08U0M     2x  256MB ( 2kB) 3.3     generic driver no supported, you shall define NF_N_DEVICES at 2 to use all capacity (Not tested)
//    NF_TYPE_K9F4G08U0A     2x  256MB ( 2kB) 3.3     generic driver no supported, you shall define NF_N_DEVICES at 2 to use all capacity (Not tested)
//    NF_TYPE_K9K4G08U0M         512MB ( 2kB) 3.3
//    NF_TYPE_K9K8G08U0M        1024MB ( 2kB) 3.3     generic driver no supported (Not tested)
//    NF_TYPE_K9K8G08U0A        1024MB ( 2kB) 3.3     generic driver no supported
//    NF_TYPE_K9W8G08U1M     2x  512MB ( 2kB) 3.3     you shall define NF_N_DEVICES at 2 to use all capacity (Not tested)
//    NF_TYPE_K9WAG08U1A     2x 1024MB ( 2kB) 3.3     generic driver no supported, you shall define NF_N_DEVICES at 2 to use all capacity (Not tested)
//    NF_TYPE_K9NBG08U5A     4x 1024MB ( 2kB) 3.3     generic driver no supported, you shall define NF_N_DEVICES at 4 to use all capacity (Not tested)
//
//    ---- M I C R O N ------
//    NF_TYPE_MT29F4G08BABWP     512MB ( 2kB) 3.3
//    NF_TYPE_MT29F2G08AAAWG     256MB ( 2kB) 3.3
//    NF_TYPE_MT29F2G08AABWP     256MB ( 2kB) 3.3     generic driver no supported
//    NF_TYPE_MT29F2G08AACWP     256MB ( 2kB) 3.3
//    NF_TYPE_MT29F2G08ABCWP     256MB ( 2kB) 1.8
//    NF_TYPE_MT29F8G08BAAWP    1024MB ( 2kB) 3.3     (Not tested)
//    NF_TYPE_MT29F16G08FAAWP 2x 1024MB ( 2kB) 3.3    you shall define NF_N_DEVICES at 2 to use all capacity (Not tested)
//    NF_TYPE_MT29F16G08FAAWC 2x 1024MB ( 2kB) 3.3    you shall define NF_N_DEVICES at 2 to use all capacity (Not tested)
//
//    ---- S T --------------
//    NF_TYPE_NAND01GW3B2A       128MB ( 2kB) 3.3     Not working: stall USB when reading.
//    NF_TYPE_NAND128W3A          16MB (512B) 3.3
//
//    ---- H Y N I X --------
//    NF_TYPE_HY27UF081G2M       128MB ( 2kB) 3.3
//    NF_TYPE_HY27UH08AG5M   2x 1024MB ( 2kB) 3.3     you shall define NF_N_DEVICES at 2 to use all capacity


#endif // _CONF_NF_H_
