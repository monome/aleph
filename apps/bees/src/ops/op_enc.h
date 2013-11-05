#ifndef _OP_ENC_H_
#define _OP_ENC_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_enc_t : encoder
typedef struct op_enc_struct {
  op_t super;
  volatile io_t val;
  volatile io_t move;
  volatile io_t step;
  volatile io_t min;
  volatile io_t max;
  volatile io_t wrap;
  volatile io_t * in_val[5];
  op_out_t outs[2];
} op_enc_t;

void op_enc_init(void* mem);

#endif // header guard
