#ifndef _ALEPH_BEES_OP_STEP_H_
#define _ALEPH_BEES_OP_STEP_H_

#include "net_monome.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_step : monome grid as a step sequencer
typedef struct op_step_struct {
  op_t super;
  op_monome_t monome;
  // inputs: mode, focus, step
  volatile io_t focus;
  volatile io_t size;
  volatile io_t* in_val[3];
  // outputs: a,b,c,d
  op_out_t outs[8];
  // internal:
  s8 s_start, s_end, s_length, s_now, s_cut;
  s8 s_start2, s_end2, s_length2, s_now2, s_cut2;
  u8 steps[4][16];
} op_step_t;

// init
void op_step_init(void* op);
// de-init
void op_step_deinit(void* op);

#endif // header guard
