#ifndef _op_harry_H_
#define _op_harry_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_harry_t : harrytiplication
typedef struct op_harry_struct {
  op_t super;
  io_t val;
  volatile io_t x;
  volatile io_t y;
  volatile io_t bang;
  volatile io_t size;
  volatile io_t* in_val[4];
  op_out_t outs[1];
} op_harry_t;

void op_harry_init(void* harry);


#endif // header guard
