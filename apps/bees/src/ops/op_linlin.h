#ifndef _op_linlin_H_
#define _op_linlin_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_linlin_t : linlintiplication
typedef struct op_linlin_struct {
  op_t super;
  io_t val;
  volatile io_t in;
  volatile io_t imin;
  volatile io_t imax;
  volatile io_t omin;
  volatile io_t omax;
  volatile io_t* in_val[5];
  s32 scale_factor;
  op_out_t outs[1];
} op_linlin_t;

void op_linlin_init(void* linlin);


#endif // header guard
