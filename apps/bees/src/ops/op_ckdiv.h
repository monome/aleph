#ifndef _BEES_OP_CKDIV_H_
#define _BEES_OP_CKDIV_H_

// aleph-avr32
#include "timers.h"

// bees
#include "net_poll.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_ckdiv_t: ckdiv
typedef struct op_ckdiv_struct {
  // superclass
  op_t super;
  // input pointers
  // enable, period, value
  volatile io_t* in_val[4];
  // state variables
  volatile io_t enable;
  volatile io_t period;
  volatile io_t value;
  volatile io_t divide;
  // outputs
  op_out_t outs[2];
  u32 tocks;
  u32 tockremainder;
  u32 ticklength;
  u16 cacheDivision;
  u16 cacheRemainder;
  // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;
} op_ckdiv_t;

// init
void op_ckdiv_init(void* op);

// de-init
void op_ckdiv_deinit(void* op);

#endif // header guard
