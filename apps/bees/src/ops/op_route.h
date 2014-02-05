#ifndef _OP_ROUTE_H_
#define _OP_ROUTE_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_split_t : switch (non-system)
typedef struct op_route_struct {
  // superclass
  op_t super;
  // state variables / inputs
  volatile io_t val;
  volatile io_t to;
  // pointers for external access
  volatile io_t* in_val[2];
  op_out_t outs[4];
} op_route_t;

extern void op_route_init(void* split4);

#endif // header guard
