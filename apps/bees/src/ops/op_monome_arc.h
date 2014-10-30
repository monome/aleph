#ifndef _ALEPH_BEES_OP_MONOME_ARC_H_
#define _ALEPH_BEES_OP_MONOME_ARC_H_

#include "net_monome.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_monome_arc
typedef struct op_marc_struct {
  op_t super;
  op_monome_t monome;
  // focus toggle
  volatile io_t focus;
  // loopback toggle
  volatile io_t loop;
  // set ring number
  volatile io_t ring;
  // set update position
  volatile io_t pos;
  // set update value
  volatile io_t val;
  // input pointer array
  volatile io_t* in_val[5];
  op_out_t outs[3];
  // internal
  u8 mRing;
  u8 vals[4];
} op_marc_t;

// init
void op_marc_init(void* op);
// de-init
void op_marc_deinit(void* op);

#endif // header guard
