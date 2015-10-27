#ifndef _op_change_H_
#define _op_change_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_change_t : filter repetition
typedef struct op_change_struct {
  op_t super;

  volatile io_t val;
  volatile io_t last;
  volatile io_t* in_val[2];

  op_out_t outs[1];
} op_change_t;

void op_change_init(void* mem);


#endif // header guard
