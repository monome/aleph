#ifndef _OP_GATE_H_
#define _OP_GATE_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_gate_t : gate
typedef struct op_gate_struct {
  op_t super;
  volatile io_t val, gate, store;
  volatile io_t * in_val[3];
  op_out_t outs[1];
} op_gate_t;
void op_gate_init(void* gate);

#endif // header guard
