#ifndef _op_shl_H_
#define _op_shl_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_mul_t : multiplication 
typedef struct op_shl_struct {
  op_t super;
  io_t val; 
  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t* in_val[3]; // a, b, btrig
  op_out_t outs[1];
} op_shl_t;

void op_shl_init(void* shl);


#endif // header guard
