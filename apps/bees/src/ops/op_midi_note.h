#ifndef _ALEPH_BEES_OP_MIDI_NOTE_H_
#define _ALEPH_BEES_OP_MIDI_NOTE_H_

#include "net_midi.h"

//--- op_midi_note_t : note and velocity. yep


typedef struct op_midi_note_struct {
  // operator base class
  op_t super;
  // midi operator abstract class
  op_midi_t midi;  
  // input: channel number
  volatile io_t* in_val[1];
  // outputs: note number, noteon velocity, noteoff velocity
  op_out_t outs[3];
  // channel, input value(negative == all channels)
  volatile io_t chanIo;
  /// actual channel number
  // this is stored separately as 8b value,
  // i don't think there's a good reason for this, but whatever
  s8 chan;
} op_midi_note_t;

// init
void op_midi_note_init(void* op);
// de-init
void op_midi_note_deinit(void* op);

#endif // h guard
