#ifndef _op_mod_H_
#define _op_mod_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_mod_t : modulus
typedef struct op_mod_struct {
  op_t super;
  io_t val; 
  volatile io_t a;
  volatile io_t b;
  volatile io_t btrig;
  volatile io_t* in_val[3]; // a, b, btrig
  op_out_t outs[1];
} op_mod_t;

void op_mod_init(void* mod);


#endif // header guard
