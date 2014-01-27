#ifndef _op_list2_H_
#define _op_list2_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_add_t : addition
typedef struct op_list2_struct {
  op_t super;
  volatile io_t val;
  volatile io_t index;
  volatile io_t i0;
  volatile io_t i1;

  volatile io_t * in_val[3];
  op_out_t outs[1];
} op_list2_t;
void op_list2_init(void* mem);

#endif // header guard
