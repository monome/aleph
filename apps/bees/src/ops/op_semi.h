#ifndef _OP_SEMI_H_
#define _OP_SEMI_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_semi_t : semigle (software switch)
typedef struct op_semi_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t note;
  // pointers for external access
  volatile io_t* in_val[1];
  op_out_t outs[1];
} op_semi_t;

extern void op_semi_init(void* semi);

#endif // header guard
