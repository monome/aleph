#ifndef _INIT_H_
#define _INIT_H_

#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "spi.h"
#include "eic.h"
#include "tc.h"
#include "intc.h"
#include "config.h"
#include "protocol.h"


extern void init_spi( void );
extern void init_control_pins( void );

#endif // header guard
