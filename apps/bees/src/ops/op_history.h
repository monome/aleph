#ifndef _op_history_H_
#define _op_history_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_history_t
typedef struct op_history_struct {
  op_t super;
  io_t val[8];
  volatile io_t in;
  volatile io_t* in_val[1];
  op_out_t outs[9];
} op_history_t;

void op_history_init(void* history);


#endif // header guard
