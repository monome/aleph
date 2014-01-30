#ifndef _op_mul_H_
#define _op_mul_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_mul_t : multiplication 
typedef struct op_mul_struct {
  op_t super;
  io_t val; 
  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t* in_val[3]; // a, b, btrig
  op_out_t outs[1];
} op_mul_t;

void op_mul_init(void* mul);


#endif // header guard
