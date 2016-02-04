#ifndef _OP_PARAM_H_
#define _OP_PARAM_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_param_t : event param
typedef struct op_param_struct {
  // superclass
  op_t super;
  // ---- input state
  // event to be timed
  volatile io_t event;
  // ---- input access pointers
  volatile io_t* in_val[1];
  // output: time
  op_out_t outs[1];
  /// additional state variables
  // interval since last event, in ticks
} op_param_t;

extern void op_param_init(void* param);

// handle input from system 
extern void op_param_sys_input(op_param_t* param, u8 val);

#endif // header guard
