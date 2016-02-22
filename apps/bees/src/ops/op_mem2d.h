#ifndef _op_mem2d_H_
#define _op_mem2d_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_mem2d_t : mem2dition
typedef struct op_mem2d_struct {
  op_t super;
  volatile io_t x;
  volatile io_t y;
  volatile io_t write;
  volatile io_t read;
  volatile io_t tog;
  volatile io_t rowsel;
  volatile io_t colsel;

  volatile io_t * in_val[5];
  op_out_t outs[3];
} op_mem2d_t;
void op_mem2d_init(void* mem);

#endif // header guard
