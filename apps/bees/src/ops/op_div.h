#ifndef _op_div_H_
#define _op_div_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_div_t : integer division 
typedef struct op_div_struct {
  op_t super;
  io_t val; 
  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t* in_val[3]; // a, b, btrig
  op_out_t outs[1];
} op_div_t;

void op_div_init(void* div);


#endif // header guard
