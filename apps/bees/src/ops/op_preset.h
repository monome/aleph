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
  // - get current index
  volatile io_t read, write;
  volatile io_t* in_val[2];
  // outputs: 
  // - current index
  //  op_out_t outs[1];
} op_preset_t;

void op_preset_init(void* preset);


#endif // header guard
