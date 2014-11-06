#ifndef _op_divr_H_
#define _op_divr_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_divr_t : integer division 
typedef struct op_divr_struct {
  op_t super;
  io_t val; 
  io_t r;
  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t* in_val[3]; // a, b, btrig
  op_out_t outs[2];
} op_divr_t;

void op_divr_init(void* div);


#endif // header guard
