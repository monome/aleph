#ifndef _OP_ENC_H_
#define _OP_ENC_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_enc_t : encoder
typedef struct op_enc_struct {
  op_t super;
  io_t val;
  volatile io_t step;
  volatile io_t min;
  volatile io_t max;
  volatile io_t * in_val[4];
  op_out_t outs[2];
  /// non-exposed state:
  s32 val32;
} op_enc_t;

extern void op_enc_init(void* mem);

extern void op_enc_sys_input(op_enc_t* enc, s8 dir);

#endif // header guard
