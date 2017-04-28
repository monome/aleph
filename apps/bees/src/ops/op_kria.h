#ifndef _ALEPH_BEES_OP_KRIA_H_
#define _ALEPH_BEES_OP_KRIA_H_

#include "net_monome.h"
#include "net_poll.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- white whale
typedef struct op_kria_struct {
  op_t super;
  op_monome_t monome;
  // inputs: mode, focus, step
  volatile io_t focus;
  volatile io_t clk;
  volatile io_t octave;
  volatile io_t tuning;
  volatile io_t* in_val[3];
  // outputs: a,b,c,d
  op_out_t outs[7];
  // internal:


    // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;

} op_kria_t;

// init
void op_kria_init(void* op);
// de-init
void op_kria_deinit(void* op);

#endif // header guard
