#ifndef _op_mem0d_H_
#define _op_mem0d_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_mem0d_t : mem0dition
typedef struct op_mem0d_struct {
  op_t super;
  volatile io_t write;
  volatile io_t read;
  volatile io_t tog;
  volatile io_t rowsel;
  volatile io_t colsel;

  volatile io_t * in_val[3];
  op_out_t outs[1];
} op_mem0d_t;
void op_mem0d_init(void* mem);

#endif // header guard
