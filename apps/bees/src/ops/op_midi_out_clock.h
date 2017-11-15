#ifndef _ALEPH_BEES_OP_MIDI_OUT_CLOCK_H_
#define _ALEPH_BEES_OP_MIDI_OUT_CLOCK_H_

#include "op.h"

/*
  note: the ordering of output nodes in the op spec
  is significant for order of execution.

  need to decide on optimal triggering behavior.
  velocity-triggered for now.
 */

//--- op_midi_out_clock_t : midi clock output
typedef struct op_midi_out_clock_struct {
  // operator base class
  op_t super;
  // input: chan, vel, num 
  volatile io_t* in_val[5];
  // outputs: none
  op_out_t outs[0];
  // output midi cable
  volatile io_t cable;
  volatile io_t tick;
  volatile io_t start;
  volatile io_t cont;
  volatile io_t stop;
} op_midi_out_clock_t;

// init
void op_midi_out_clock_init(void* op);
// de-init
void op_midi_out_clock_deinit(void* op);

#endif // h guard
