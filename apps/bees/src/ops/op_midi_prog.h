#ifndef _ALEPH_BEES_OP_MIDI_PROG_H_
#define _ALEPH_BEES_OP_MIDI_PROG_H_

#include "net_midi.h"

//--- op_midi_prog_t : it is a midi control change operator, yep


typedef struct op_midi_prog_struct {
  // operator base class
  op_t super;
  // midi operator abstract class
  op_midi_t midi;  
  // input: channel, control num
  volatile io_t* in_val[1];
  // outputs: value
  op_out_t outs[1];
  // target channel (-1 == all channels)
  volatile io_t chan;
} op_midi_prog_t;

// init
void op_midi_prog_init(void* op);
// de-init
void op_midi_prog_deinit(void* op);

#endif // h guard
