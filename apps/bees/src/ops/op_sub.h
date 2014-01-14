#ifndef _op_sub_H_
#define _op_sub_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_sub_t : subtraction
typedef struct op_sub_struct {
  op_t super;
  // state
  volatile io_t val; 
  //inputs:
  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t * in_val[3]; // a, b, btrig
  op_out_t outs[1];
} op_sub_t;
void op_sub_init(void* mem);

#endif // header guard
