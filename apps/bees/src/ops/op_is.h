#ifndef _op_is_H_
#define _op_is_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_is_t : comparisons
typedef struct op_is_struct {
  op_t super;

  io_t eq;
  io_t ne;
  io_t gt;
  io_t lt;

  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t edge;
  volatile io_t* in_val[4];

  op_out_t outs[4];
} op_is_t;

void op_is_init(void* is);


#endif // header guard