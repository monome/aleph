#ifndef _ALEPH_BEES_OP_MONOME_GRID_RAW_H_
#define _ALEPH_BEES_OP_MONOME_GRID_RAW_H_

#include "net_monome.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_monome_grid_raw : monome grid as a simple bank of switches 
typedef struct op_mgrid_raw_struct {
  op_t super;
  op_monome_t monome;
  // inputs: toggle mode, focus
  volatile io_t focus;
  volatile io_t tog;
  volatile io_t mono;
  volatile io_t* in_val[3];
  // outputs: x , y, z
  op_out_t outs[3];
  // internal:
  u32 lastPos;
} op_mgrid_raw_t;

// init
void op_mgrid_raw_init(void* op);
// de-init
void op_mgrid_raw_deinit(void* op);

#endif // header guard
