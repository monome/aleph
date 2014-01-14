#ifndef _OP_SW_H_
#define _OP_SW_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_sw_t : switch
typedef struct op_sw_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t state;
  volatile io_t mul;
  volatile io_t tog;
  // pointers for external access
  volatile io_t* in_val[3];
  op_out_t outs[1];
} op_sw_t;

extern void op_sw_init(void* sw);

// handle input from system 
extern void op_sw_sys_input(op_sw_t* sw, u8 val);

#endif // header guard
