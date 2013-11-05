/*****************************************************************************
 *
 * \file
 *
 * \brief UC3-L0-Xplained board LEDs support package.
 *
 * This file contains definitions and services related to the LED features of
 * the UC3L0-Xplained board.
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
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


#include "board.h"


//! Structure describing LED hardware connections.
typedef const struct
{
  struct
  {
    U32 PORT;     //!< LED GPIO port.
    U32 PIN_MASK; //!< Bit-mask of LED pin in GPIO port.
  } GPIO; //!< LED GPIO descriptor.
  struct
  {
    S32 CHANNEL;  //!< LED PWM channel (< 0 if N/A).
    S32 FUNCTION; //!< LED pin PWM function (< 0 if N/A).
  } PWM;  //!< LED PWM descriptor.
  bool polarity;
} tLED_DESCRIPTOR;


//! Hardware descriptors of all LEDs.
static tLED_DESCRIPTOR LED_DESCRIPTOR[LED_COUNT] =
{
#define INSERT_LED_DESCRIPTOR(LED_NO, unused)                 \
  {                                                           \
    {LED##LED_NO##_GPIO / 32, 1 << (LED##LED_NO##_GPIO % 32)},\
    {LED##LED_NO##_PWMA,      LED##LED_NO##_PWMA_FUNCTION   },\
	LED##LED_NO##_POLARITY                                    \
  },
  MREPEAT(LED_COUNT, INSERT_LED_DESCRIPTOR, ~)
#undef INSERT_LED_DESCRIPTOR
};


//! Saved state of all LEDs.
static volatile U32 LED_State = (1 << LED_COUNT) - 1;


U32 LED_Read_Display(void)
{
  return LED_State;
}


U32 LED_Read_Display_Mask(U32 mask)
{
  return Rd_bits(LED_State, mask);
}


void LED_Display_Mask(U32 mask, U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
   U8 led_shift;

  // Make sure only existing LEDs are specified.
  mask &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Wr_bits(LED_State, mask, leds);

  // While there are specified LEDs left to manage...
  while (mask) {
    // Select the next specified LED and set it to the requested state.
    led_shift = 1 + ctz(mask);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    leds >>= led_shift - 1;

    if ((leds & 1) ^ (led_descriptor->polarity)) {
		led_gpio_port->ovrc = led_descriptor->GPIO.PIN_MASK;
	} else {
		led_gpio_port->ovrs = led_descriptor->GPIO.PIN_MASK;
	}

	led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK; //set as output
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK; //enable IO
    leds >>= 1;
    mask >>= led_shift;
  }
}

void LED_Display(U32 leds)
{
  LED_Display_Mask(leds, (U32)-1);
}

bool LED_Test(U32 leds)
{
  return Tst_bits(LED_State, leds);
}


void LED_Off(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Clr_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds) {
    // Select the next specified LED and turn it off.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];

	if (led_descriptor->polarity) {
		led_gpio_port->ovrc = led_descriptor->GPIO.PIN_MASK;
    } else {
		led_gpio_port->ovrs = led_descriptor->GPIO.PIN_MASK;
	}

    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK; //set as output
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK; //enable IO
    leds >>= led_shift;
  }
}


void LED_On(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Set_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and turn it on.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];

	if (led_descriptor->polarity) {
		led_gpio_port->ovrs = led_descriptor->GPIO.PIN_MASK;
    } else {
		led_gpio_port->ovrc = led_descriptor->GPIO.PIN_MASK;
	}

    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK; //set as output
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK; //enable IO
    leds >>= led_shift;
  }
}


void LED_Toggle(U32 leds)
{
  // Use the LED descriptors to get the connections of a given LED to the MCU.
  tLED_DESCRIPTOR *led_descriptor = &LED_DESCRIPTOR[0] - 1;
  volatile avr32_gpio_port_t *led_gpio_port;
  U8 led_shift;

  // Make sure only existing LEDs are specified.
  leds &= (1 << LED_COUNT) - 1;

  // Update the saved state of all LEDs with the requested changes.
  Tgl_bits(LED_State, leds);

  // While there are specified LEDs left to manage...
  while (leds)
  {
    // Select the next specified LED and toggle it.
    led_shift = 1 + ctz(leds);
    led_descriptor += led_shift;
    led_gpio_port = &AVR32_GPIO.port[led_descriptor->GPIO.PORT];
    led_gpio_port->ovrt  = led_descriptor->GPIO.PIN_MASK;
    led_gpio_port->oders = led_descriptor->GPIO.PIN_MASK; //set as output
    led_gpio_port->gpers = led_descriptor->GPIO.PIN_MASK; //enable IO
    leds >>= led_shift;
  }
}


U32 LED_Read_Display_Field(U32 field)
{
  return Rd_bitfield(LED_State, field);
}


void LED_Display_Field(U32 field, U32 leds)
{
  // Move the bit-field to the appropriate position for the bit-mask.
  LED_Display_Mask(field, leds << ctz(field));
}


U8 LED_Get_Intensity(U32 led)
{
  // Not implemented
  return 0;
}

void LED_Set_Intensity(U32 leds, U8 intensity)
{
  // Not implemented. Please use the PWMA-driver instead.
}

