#ifndef _INIT_AVR_H_
#define _INIT_AVR_H_

#include "compiler.h"

//extern void init_avr(void); 
extern void init_clocks(void);
extern void init_usart(void);
extern void init_gpio(void);
extern void init_tc(volatile avr32_tc_t *tc);

#endif // _INIT_AVR_H_
