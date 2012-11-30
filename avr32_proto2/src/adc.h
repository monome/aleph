#ifndef _ALEPH_AVR32_ADC_H_
#define _ALEPH_AVR32_ADC_H_

#include "compiler.h"

// setup ad7923
extern void init_adc(void);

// perform a conversion on all 4 channels
//tern void adc_convert(U16 * dst[4]);

// perform conversion, check for changes, and post events
extern void adc_poll(void);

#endif

