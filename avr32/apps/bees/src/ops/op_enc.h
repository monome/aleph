#ifndef _OP_ENC_H_
#define _OP_ENC_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_enc_t : encoder
typedef struct op_enc_struct {
  op_t super;
  io_t val;
  io_t move, step, min, max, wrap;
  io_t * in_val[5];
  op_out_t outs[2];
} op_enc_t;
void op_enc_init(void* mem);

#endif // header guard
