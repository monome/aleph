#ifndef _OP_ADD_H_
#define _OP_ADD_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_add_t : addition
typedef struct op_add_struct {
  op_t super;
  io_t val; 
  io_t a, b, btrig;
  io_t * in_val[3];
  op_out_t outs[1];
} op_add_t;
void op_add_init(void* mem);

#endif // header guard
