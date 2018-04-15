#ifndef _op_hist2_H_
#define _op_hist2_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_hist2_t
typedef struct op_hist2_struct {
  op_t super;
  io_t val[2];
  volatile io_t in;
  volatile io_t* in_val[1];
  op_out_t outs[2];
} op_hist2_t;

void op_hist2_init(void* hist2);

#endif // header guard
