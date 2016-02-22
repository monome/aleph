#ifndef _op_iter_H_
#define _op_iter_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_iter_t : iterition
typedef struct op_iter_struct {
  op_t super;
  volatile io_t val; 
  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t * in_val[3]; // a, b, btrig
  op_out_t outs[1];
} op_iter_t;
void op_iter_init(void* mem);

#endif // header guard
