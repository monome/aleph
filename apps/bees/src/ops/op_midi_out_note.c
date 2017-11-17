// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_out_note.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_out_note_instring =  "CABLE\0  CHAN\0   NUM\0    VEL\0    PITCH\0  ";
static const char* op_midi_out_note_outstring = "";
static const char* op_midi_out_note_opstring = "MOUTNO";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_midi_out_note_in_cable(op_midi_out_note_t* mout, const io_t val);
static void op_midi_out_note_in_chan(op_midi_out_note_t* mout, const io_t val);
static void op_midi_out_note_in_vel(op_midi_out_note_t* mout, const io_t val);
static void op_midi_out_note_in_num(op_midi_out_note_t* mout, const io_t val);
static void op_midi_out_note_in_pitch(op_midi_out_note_t* mout, const io_t val);

// pickles
static u8* op_midi_out_note_pickle(op_midi_out_note_t* mout, u8* dst);
static const u8* op_midi_out_note_unpickle(op_midi_out_note_t* mout, const u8* src);

// build and send a midi serial packet
static void op_midi_out_note_send_packet( op_midi_out_note_t* mout );

// input func pointer array
static op_in_fn op_midi_out_note_in_fn[5] = {
  (op_in_fn)&op_midi_out_note_in_cable,
  (op_in_fn)&op_midi_out_note_in_chan,
  (op_in_fn)&op_midi_out_note_in_num,
  (op_in_fn)&op_midi_out_note_in_vel,
  (op_in_fn)&op_midi_out_note_in_pitch,
};

//-------------------------------------------------
//----- extern function definition
void op_midi_out_note_init(void* mem) {
  //  print_dbg("\r\n op_midi_out_note_init ");
  op_midi_out_note_t* op = (op_midi_out_note_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_midi_out_note_inc_fn;
  op->super.in_fn = op_midi_out_note_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_midi_out_note_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_midi_out_note_unpickle);

  // superclass state

  op->super.type = eOpMidiOutNote;
  //  op->super.flags |= (1 << eOpFlagMidiIn);

  op->super.numInputs = 5;
  op->super.numOutputs = 0;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_out_note_opstring;
  op->super.inString = op_midi_out_note_instring;
  op->super.outString = op_midi_out_note_outstring;

  op->in_val[0] = &(op->cable);
  op->in_val[1] = &(op->chan);
  op->in_val[2] = &(op->num);
  op->in_val[3] = &(op->vel);
  op->in_val[4] = &(op->pitch);

  op->cable = 0;
  op->chan = OP_ONE;
  op->num = 0;
  op->vel = 0;
  op->pitch = 0;

}

// de-init
void op_midi_out_note_deinit(void* op) {
  //...
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_midi_out_note_in_chan(op_midi_out_note_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < -1) { op->chan = -1;  }
  else if(v > 15) { op->chan = 15; } 
  else { op->chan = v; }
}
static void op_midi_out_note_in_cable(op_midi_out_note_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < 0) {
    op->cable = 0;
  }
  else if(v > 16) {
    op->cable = 16;
  } 
  else {
    op->cable = v;
  }
}


static void op_midi_out_note_in_num(op_midi_out_note_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < 0) { op->num = 0; }
  else if (v > 127) { op->num = 127; }
  else { 
    op->num = v;
  }
}

static void op_midi_out_note_in_vel(op_midi_out_note_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < 0) { op->vel = 0; }
  else if (v > 127) { op->vel = 127; }
  else { op->vel = v; }

  op_midi_out_note_send_packet(op);
}

static void op_midi_out_note_in_pitch(op_midi_out_note_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < 0 - MIDI_BEND_ZERO) {
    op->pitch = 0 - MIDI_BEND_ZERO;
  }
  else if (v >= MIDI_BEND_ZERO ) {
    op->pitch = MIDI_BEND_ZERO - 1;
  }
  else { 
    op->pitch = v;
  }
  s16 midi_pitch = op->pitch + MIDI_BEND_ZERO;
  u8 pack[3];
  pack[0] = 0b1110 << 4;
  pack[0] |= (u8)(op->chan & 0x0f);
  pack[1] = 0x7f & midi_pitch; // bits 0 - 7
  pack[2] = 0x7f & (midi_pitch >> 7); // bits 8 - 14
#ifndef BEEKEEP
  midi_write_packet(op->cable, pack);
#endif
}

// build and send a midi serial packet
void op_midi_out_note_send_packet( op_midi_out_note_t* mout ) {
  u8 pack[3];
  if(mout->vel == 0) {
    // note on/off
    pack[0] = 0x80;
  } else {
    pack[0] = 0x90;
  }
  pack[0] |= (u8)(mout->chan & 0x0f);
  pack[1] = (u8)(mout->num);
  pack[2] = (u8)(mout->vel);

#ifndef BEEKEEP
  midi_write_packet(mout->cable, pack);
#endif
}

// pickle / unpickle
u8* op_midi_out_note_pickle(op_midi_out_note_t* mout, u8* dst) {
  dst = pickle_io(mout->cable, dst);
  dst = pickle_io(mout->chan, dst);
  dst = pickle_io(mout->num, dst);
  return dst;
}

const u8* op_midi_out_note_unpickle(op_midi_out_note_t* mout, const u8* src) {
  src = unpickle_io(src, (u32*)&(mout->cable));
  src = unpickle_io(src, (u32*)&(mout->chan));
  src = unpickle_io(src, (u32*)&(mout->num));
  mout->chan = op_to_int(mout->chan);
  return src;
}
