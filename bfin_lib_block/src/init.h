
/*!
 init.h
 bfin_lib
 aleph

 small, independent, low-level hardware init functions go here 

 */

#ifndef _BFIN_INIT_H_
#define _BFIN_INIT_H_

#include "init.h"
#include "gpio.h"

// initialize the clock / PLL
void init_clock(void);
// initialize memory controller
void init_ebiu(void);
// initialize programmable flags (GPIO)
void init_flags(void);
// configure audio codec ( AD1939 )
void init_codec(void);

#endif
