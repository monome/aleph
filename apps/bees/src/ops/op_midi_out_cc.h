#ifndef _ALEPH_BEES_OP_MIDI_OUT_CC_H_
#define _ALEPH_BEES_OP_MIDI_OUT_CC_H_

#include "op.h"

/*
  note: the ordering of output nodes in the op spec
  is significant for order of execution.

  need to decide on optimal triggering behavior.
  velocity-triggered for now.
 */

//--- op_midi_out_cc_t : midi cc output
typedef struct op_midi_out_cc_struct {
  // operator base class
  op_t super;
  // input: chan, vel, num 
  volatile io_t* in_val[3];
  // outputs: none
  op_out_t outs[0];
  // output channel (-1 == all channels)
  volatile io_t chan;
  // output cc num
  volatile io_t num;
  // output value
  volatile io_t val;
} op_midi_out_cc_t;

// init
void op_midi_out_cc_init(void* op);
// de-init
void op_midi_out_cc_deinit(void* op);

#endif // h guard
