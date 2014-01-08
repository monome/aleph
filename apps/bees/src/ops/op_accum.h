#ifndef _OP_ACCUM_H_
#define _OP_ACCUM_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_accum_t : accumulator
typedef struct op_accum_struct {
  // superclass
  op_t super;
  // state variables
  // increment
  volatile io_t inc;
  // current value
  volatile io_t val;
  // min
  volatile io_t min;
  // max 
  volatile io_t max;
  // toggle wrap/saturate
  volatile io_t wrap;
  // pointers for external access
  volatile io_t* in_val[5];
  // outputs: value, wrap
  op_out_t outs[2];
} op_accum_t;

extern void op_accum_init(void* accum);


#endif // header guard
