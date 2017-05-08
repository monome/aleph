#ifndef _OP_MAGINC_H_
#define _OP_MAGINC_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_maginc_t : magincgle (software switch)
typedef struct op_maginc_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t inc;
  // pointers for external access
  volatile io_t* in_val[1];
  op_out_t outs[1];
} op_maginc_t;

extern void op_maginc_init(void* maginc);

#endif // header guard
