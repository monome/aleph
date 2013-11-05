/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
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

#ifndef _USB_SPECIFIC_REQUEST_H_
#define _USB_SPECIFIC_REQUEST_H_

//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_DEVICE_FEATURE == false
  #error usb_specific_request.h is #included although USB_DEVICE_FEATURE is disabled
#endif


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

extern  const S_usb_device_descriptor usb_dev_desc;
extern  const S_usb_user_configuration_descriptor usb_conf_desc;
//extern const S_usb_user_configuration_descriptor  usb_other_conf_desc;
//extern const S_usb_device_qualifier_descriptor    usb_qual_desc;
extern  const S_usb_manufacturer_string_descriptor usb_user_manufacturer_string_descriptor;
extern  const S_usb_product_string_descriptor usb_user_product_string_descriptor;
extern  const S_usb_serial_number usb_user_serial_number;
extern  const S_usb_language_id usb_user_language_id;

//! Usb HID Report Descriptor Keyboard
#define USB_HID_REPORT_DESC_KBD  39
extern const uint8_t usb_hid_report_descriptor_kbd[USB_HID_REPORT_DESC_KBD];


//! @defgroup specific_request USB device specific requests
//! @{

//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
//! The core of this function should be correctly rewritten depending on the USB device
//! application characteristics (the USB device application has specific endpoint
//! configuration).
//!
extern void  usb_user_endpoint_init( uint8_t );

//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns true when the
//! request is processed. This function returns false if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
extern bool  usb_user_read_request( uint8_t, uint8_t );
extern uint8_t    usb_user_interface_get( uint16_t wInterface );
extern void  usb_user_interface_reset( uint16_t wInterface, uint8_t alternate_setting );
extern bool  usb_user_get_descriptor( uint8_t , uint8_t );
//! @}

//! Speaker control
extern void  audio_speaker_set_mute(void);
extern void  audio_speaker_get_mute(void);
extern void  audio_speaker_set_volume(void);
extern void  audio_speaker_get_vol_cur(void);
extern void  audio_speaker_get_vol_min(void);
extern void  audio_speaker_get_vol_max(void);
extern void  audio_speaker_get_vol_res(void);
extern void  audio_speaker_set_sample_freq(void);

//! Micro control
extern void  audio_micro_set_mute(void);
extern void  audio_micro_get_mute(void);
extern void  audio_micro_set_volume(void);
extern void  audio_micro_get_vol_cur(void);
extern void  audio_micro_get_vol_min(void);
extern void  audio_micro_get_vol_max(void);
extern void  audio_micro_get_vol_res(void);

// ____ T Y P E  D E F I N I T I O N _______________________________________



#endif  // _USB_SPECIFIC_REQUEST_H_
