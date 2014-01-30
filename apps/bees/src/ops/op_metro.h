#ifndef _BEES_OP_METRO_H_
#define _BEES_OP_METRO_H_

// aleph-avr32
#include "timers.h"

// bees
#include "net_poll.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_metro_t: metro
typedef struct op_metro_struct {
  // superclass
  op_t super;
  // input pointers
  // enable, period, value
  volatile io_t* in_val[3];
  // state variables
  volatile io_t enable;
  volatile io_t period;
  volatile io_t value;
  // outputs
  op_out_t outs[1];
  // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;
} op_metro_t;

// init
void op_metro_init(void* op);

// de-init
void op_metro_deinit(void* op);

#endif // header guard
