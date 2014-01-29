#ifndef _op_bits_H_
#define _op_bits_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_bits_h: bin to dec and back
typedef struct op_bits_struct {
  op_t super;
  volatile io_t val;
  volatile io_t index;
  volatile io_t i0;
  volatile io_t i1;
  volatile io_t i2;
  volatile io_t i3;
  volatile io_t i4;
  volatile io_t i5;
  volatile io_t i6;
  volatile io_t i7;

  volatile io_t * in_val[9];
  op_out_t outs[9];
} op_bits_t;
void op_bits_init(void* mem);

#endif // header guard
