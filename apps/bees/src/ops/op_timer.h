#ifndef _OP_TIMER_H_
#define _OP_TIMER_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_timer_t : event timer
typedef struct op_timer_struct {
  // superclass
  op_t super;
  // ---- input state
  // event to be timed
  volatile io_t event;
  volatile io_t reset;
  // ---- input access pointers
  volatile io_t* in_val[2];
  // output: time
  op_out_t outs[1];
  // last event ticks
  u32 ticks;
} op_timer_t;

extern void op_timer_init(void* timer);

// handle input from system 
extern void op_timer_sys_input(op_timer_t* timer, u8 val);

#endif // header guard
