#ifndef _OP_TOG_H_
#define _OP_TOG_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_tog_t : toggle (software switch)
typedef struct op_tog_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t state;
  volatile io_t mul;
  // pointers for external access
  volatile io_t* in_val[2];
  op_out_t outs[1];
} op_tog_t;

extern void op_tog_init(void* tog);

#endif // header guard
