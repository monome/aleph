#ifndef _op_mem1d_H_
#define _op_mem1d_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_mem1d_t : mem1dition
typedef struct op_mem1d_struct {
  op_t super;
  volatile io_t n;
  volatile io_t write;
  volatile io_t read;
  volatile io_t tog;
  volatile io_t rowsel;
  volatile io_t colsel;

  volatile io_t * in_val[4];
  op_out_t outs[2];
} op_mem1d_t;
void op_mem1d_init(void* mem);

#endif // header guard
