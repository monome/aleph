#ifndef _ALEPH_BEES_OP_RANDPAT_H_
#define _ALEPH_BEES_OP_RANDPAT_H_

//aleph-avr32
#include "timers.h"

//Bees
#include "net_poll.h"
#include "types.h"
#include "op.h"
#include "op_math.h"

typedef struct op_randpat_struct {
  op_t super;

  //Inputs: channel, tick, velocity, min, max, random, enable, period
  volatile io_t chan;
  volatile io_t tick;
  volatile io_t vel;
  volatile io_t min;
  volatile io_t max;
  volatile io_t rnd;
  volatile io_t enable;
  volatile io_t period;
  volatile io_t* in_val[8];

  //Outputs: note
  op_out_t outs[1];

  //Timer Data
  softTimer_t timer;

  //Polled Operator Superclass
  op_poll_t op_poll;
} op_randpat_t;

u32 a,c,x;

//Init
void op_randpat_init(void* op);
//De-init
void op_randpat_deinit(void* op);

#endif //Header Guard
