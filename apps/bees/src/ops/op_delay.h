#ifndef _BEES_OP_DELAY_H_
#define _BEES_OP_DELAY_H_

// aleph-avr32
#include "timers.h"

// bees
#include "net_poll.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_delay_t: delay
typedef struct op_delay_struct {
  // superclass
  op_t super;
  // input pointers
  // value, time, clear
  volatile io_t* in_val[3];
  // state variables
  volatile io_t val;
  volatile io_t ms;
  volatile io_t clear;
  // outputs
  op_out_t outs[1];
  // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;
} op_delay_t;

// init
void op_delay_init(void* op);

// de-init
void op_delay_deinit(void* op);

#endif // header guard
