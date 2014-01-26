#ifndef _op_logic_H_
#define _op_logic_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_logic_t : logic operations
typedef struct op_logic_struct {
  op_t super;

  io_t v_and;
  io_t v_or;
  io_t v_xor;

  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t edge;
  volatile io_t invert;
  volatile io_t* in_val[5];

  op_out_t outs[3];
} op_logic_t;

void op_logic_init(void* is);


#endif // header guard