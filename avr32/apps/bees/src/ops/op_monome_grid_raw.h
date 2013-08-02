#ifndef _ALEPH_BEES_OP_MONOME_GRID_RAW_H_
#define _ALEPH_BEES_OP_MONOME_GRID_RAW_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_monome_grid_raw : monome as a simple bank of switches 
typedef struct op_mgrid_raw_struct {
  op_t super;
  // inputs: toggle mode, focus
  io_t focus, tog;
  io_t* in_val[2];
  // outputs: switch x , y, z
  op_out_t outs[3];
} op_mgrid_raw_t;

// init
void op_mgrid_raw_init(void* op);

#endif // header guard
