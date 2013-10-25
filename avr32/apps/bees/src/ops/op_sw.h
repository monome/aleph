#ifndef _OP_SW_H_
#define _OP_SW_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_sw_t : switch
typedef struct op_sw_struct {
  // superclass
  op_t super;
  // state variables
  io_t state, mul, tog;
  // pointers for external access
  io_t* in_val[3];
  op_out_t outs[1];
} op_sw_t;

void op_sw_init(void* sw);

#endif // header guard
