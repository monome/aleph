#ifndef _OP_SPLIT4_H_
#define _OP_SPLIT4_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_split_t : switch (non-system)
typedef struct op_split4_struct {
  // superclass
  op_t super;
  // state variables / inputs
  volatile io_t val;
  // pointers for external access
  volatile io_t* in_val[1];
  op_out_t outs[4];
} op_split4_t;

extern void op_split4_init(void* split4);

#endif // header guard
