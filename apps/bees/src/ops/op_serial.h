#ifndef _OP_SERIAL_H_
#define _OP_SERIAL_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_serial_t : comparisons
typedef struct op_serial_struct {
  op_t super;

  volatile io_t a;
  volatile io_t b;
  volatile io_t* in_val[2];

  op_out_t outs[];
} op_serial_t;

extern void op_serial_init(void* op);
extern void op_serial_deinit(void* op);


#endif // header guard
