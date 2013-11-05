/**************************************************************************
 *
 * \file
 *
 * \brief Management of the USB host mouse HID task.
 *
 * This file manages the USB host mouse HID task.
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


#ifndef _HOST_AUDIO_TASK_H_
#define _HOST_AUDIO_TASK_H_


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"

#if USB_HOST_FEATURE == false
  #error host_audio_task.h is #included although USB_HOST_FEATURE is disabled
#endif


#include "usb_host_task.h"


//_____ M A C R O S ________________________________________________________

#define Is_host_audio_configured()  (audio_connected && !Is_host_suspended())


//_____ D E F I N I T I O N S ______________________________________________

#define     MUTE_ON                         true
#define     MUTE_OFF                        false

//! Define for each 'feature unit' the max numbers of bmControls
#define MAX_BMA_CONTROLS 4

typedef struct
{
   uint8_t  unit;
   uint8_t  n_bmaControls;
   uint8_t  bmaControls[MAX_BMA_CONTROLS];
} cs_feature_unit_t;

//! Define for each 'feature unit' the max numbers of bmaControls
//! One of them is used by the microphone!
#define MAX_FEATURE_UNITS 3

typedef struct
{
  volatile cs_feature_unit_t g_cs_feature[MAX_FEATURE_UNITS];
  volatile uint8_t                g_cs_num_features_unit;

  volatile uint32_t g_sample_freq[      MAX_INTERFACE_SUPPORTED];
  volatile uint8_t  g_n_channels[       MAX_INTERFACE_SUPPORTED];
  volatile uint8_t  g_n_bits_per_sample[MAX_INTERFACE_SUPPORTED];
           uint16_t byte_count;

  uint8_t  pipe_audio_in;
  uint8_t  interf_audio_stream;

  bool sample_rate_set;
} host_audio_task_data_t;

//_____ D E C L A R A T I O N S ____________________________________________

extern host_audio_task_data_t *host_audio_task_data;
extern volatile bool audio_connected;

extern void     host_audio_set_cur_mute(uint16_t unit, bool cs_mute);
extern uint16_t host_audio_get_max(     uint16_t unit, uint16_t channel_number);
extern void     host_audio_set_cur(     uint16_t unit, uint16_t channel_number, uint16_t volume);
extern void     host_set_sampling_rate( uint16_t endpoint, uint32_t sampling_rate);
extern void     host_audio_task_init(void);
#ifdef FREERTOS_USED
extern void     host_audio_task(void *pvParameters);
#else
extern void     host_audio_task(void);
#endif
extern void     host_audio_set_alt_interface(void);
extern void     host_sof_action(void);


#endif  // _HOST_AUDIO_TASK_H_
