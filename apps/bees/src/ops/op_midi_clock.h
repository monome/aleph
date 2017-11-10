#ifndef _ALEPH_BEES_OP_MIDI_CLOCK_H_
#define _ALEPH_BEES_OP_MIDI_CLOCK_H_

#include "net_midi.h"

//--- op_midi_clock_t : it is a midi control change operator, yep


typedef struct op_midi_clock_struct {
  // operator base class
  op_t super;
  // midi operator abstract class
  op_midi_t midi;  
  // input: channel, control num
  volatile io_t* in_val[0];
  // outputs: value
  op_out_t outs[4];
} op_midi_clock_t;

// init
void op_midi_clock_init(void* op);
// de-init
void op_midi_clock_deinit(void* op);

#endif // h guard
