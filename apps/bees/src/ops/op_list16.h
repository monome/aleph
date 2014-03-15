#ifndef _OP_LIST16_H_
#define _OP_LIST16_H_

#include "types.h"
#include "op.h"
#include "op_math.h"

//--- op_add_t : addition
typedef struct op_list16_struct {
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
  volatile io_t i8;
  volatile io_t i9;
  volatile io_t i10;
  volatile io_t i11;
  volatile io_t i12;
  volatile io_t i13;
  volatile io_t i14;
  volatile io_t i15;

  volatile io_t * in_val[17];
  op_out_t outs[1];
} op_list16_t;
void op_list16_init(void* mem);

#endif // header guard
