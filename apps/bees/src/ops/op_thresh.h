#ifndef _OP_THRESH_H_
#define _OP_THRESH_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_thresh_t : threshitch
typedef struct op_thresh_struct {
  // superclass
  op_t super;
  // state variables
  volatile io_t state;
  volatile io_t lim;
  // pointers for external access
  volatile io_t* in_val[2];
  // outs: hi, lo
  op_out_t outs[2];
} op_thresh_t;

extern void op_thresh_init(void* thresh);

// handle input from system 
extern void op_thresh_sys_input(op_thresh_t* thresh, u8 val);

#endif // header guard
