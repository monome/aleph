#ifndef _OP_CPU_H_
#define _OP_CPU_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_mod_t : modulus
typedef struct op_cpu_struct {
  op_t super;
  io_t trig;
  volatile io_t* in_val[1];
  op_out_t outs[2];
} op_cpu_t;

void op_cpu_init(void* cpu);


#endif // header guard
