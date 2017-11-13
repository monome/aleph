#ifndef _ALEPH_BEES_OP_CASCADES_H_
#define _ALEPH_BEES_OP_CASCADES_H_

#include "net_monome.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_step : monome grid as a complex cascade counter
typedef struct op_cascades_struct {
  op_t super;
  op_monome_t monome;
  // inputs: mode, focus, step
  volatile io_t focus;
  volatile io_t size;
  volatile io_t dummy;
  volatile io_t* in_val[3];
  // outputs: a,b,c,d
  op_out_t outs[8];
  // internal:
  s8 positions[8];
  s8 points[8];
  s8 points_save[8];
  u8 triggers[8];
  u8 trig_dests[8];
  u8 rules[8];
  u8 rule_dests[8];

  u8 edit_row;
  u8 key_count;
  u8 mode;
  u8 prev_mode;

  u8 XSIZE;

} op_cascades_t;

// init
void op_cascades_init(void* op);
// de-init
void op_cascades_deinit(void* op);

#endif // header guard
