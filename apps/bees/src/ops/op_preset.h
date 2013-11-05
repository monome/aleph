#ifndef _ALEPH_BEES_OP_PRESET_H_
#define _ALEPH_BEES_OP_PRESET_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_sw_t : switch
typedef struct op_preset_struct {
  op_t super;
// inputs:
  // - set and load index
  // - set and store index
  // - re-load current index
  // - re-store current index
  // - get current index
  io_t read, write, reread, rewrite, cur;
  io_t* in_val[5];
  // outputs: 
  // - current index
  op_out_t outs[1];
} op_preset_t;
void op_preset_init(op_preset_t* preset);


#endif // header guard
