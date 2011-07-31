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


//////////// typedefs

typedef struct {
	unsigned char pos_now, pos_old;

	int value;
	int min, max;
	float step, multiply;
} enc_t;

//////////// static variables

static enc_t enc[4];
static unsigned int param_delivery[3];

const int enc_map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };


////////////  static function prototypes

static void int_handler_port0_line0( void );


static void init_spi( void );
void HandleEncInterrupt( U8 pin );
static inline void checkEncInterruptPin(const U8 pin);
static void register_interrupts( void );
static void init_enc( void );


#endif // header guard

