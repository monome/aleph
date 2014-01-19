#ifndef _ALEPH_AVR32_ADC_H_
#define _ALEPH_AVR32_ADC_H_

//#include "compiler.h"

// setup ad7923
extern void init_adc(void);

// perform conversion, check for changes, and post events
extern void adc_poll(void);

#endif

