#ifndef _OP_MUL_H_
#define _OP_MUL_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_mul_t : multiplication 
typedef struct op_mul_struct {
  op_t super;
  io_t val; 
  io_t a, b, btrig;
  io_t * in_val[3];
  op_out_t outs[1];
} op_mul_t;
void op_mul_init(void* mul);

#endif // header guard
