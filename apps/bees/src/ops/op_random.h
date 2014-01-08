#ifndef _op_random_H_
#define _op_random_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_add_t : addition
typedef struct op_random_struct {
  op_t super;
  volatile io_t val; 
  volatile io_t min;
  volatile io_t max;
  volatile io_t trig;
  volatile io_t * in_val[3]; // min, max, trig
  op_out_t outs[1];

  u32 a,c,x;
} op_random_t;
void op_random_init(void* mem);

#endif // header guard