// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_note.h"


//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_note_instring = "CHAN\0   ";
static const char* op_midi_note_outstring = "NUM\0    VELON\0  PITCH\0  ";
static const char* op_midi_note_opstring = "MIDINOTE";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
//static void op_midi_note_inc_fn(op_midi_note_t* grid, const s16 idx, const io_t inc);
static void op_midi_note_in_chan(op_midi_note_t* mnote, const io_t val);

// pickles
static u8* op_midi_note_pickle(op_midi_note_t* mnote, u8* dst);
static const u8* op_midi_note_unpickle(op_midi_note_t* mnote, const u8* src);

/// midi event handler
static void op_midi_note_handler(op_midi_note_t* op, u8 ch, u8 num, u8 vel);
static void op_midi_pitch_bend_handler(op_midi_note_t* op, u8 ch, u16 pitch);

// input func pointer array
static op_in_fn op_midi_note_in_fn[1] = {
  (op_in_fn)&op_midi_note_in_chan,
};

//-------------------------------------------------
//----- extern function definition
void op_midi_note_init(void* mem) {
  //  print_dbg("\r\n op_midi_note_init ");
  op_midi_note_t* op = (op_midi_note_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_midi_note_inc_fn;
  op->super.in_fn = op_midi_note_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_midi_note_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_midi_note_unpickle);

  //--- midi

  net_midi_init(&(op->midi));
  op->midi.handler.note_on = (net_midi_note_on_t)&op_midi_note_handler;
  op->midi.handler.note_off = (net_midi_note_off_t)&op_midi_note_handler;
  op->midi.handler.pitch_bend = (net_midi_pitch_bend_t)&op_midi_pitch_bend_handler;
  // superclass state
  op->midi.sub = op;

  op->midi.handler.channel_pressure = NULL; //add channel pressure & pitch bend maybe???

  op->super.type = eOpMidiNote;
  op->super.flags |= (1 << eOpFlagMidiIn);

  op->super.numInputs = 1;
  op->super.numOutputs = 3;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_note_opstring;
  op->super.inString = op_midi_note_instring;
  op->super.outString = op_midi_note_outstring;

  op->in_val[0] = &(op->chanIo);
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = 0;

  op->chan = -1;
  op->chanIo = OP_NEG_ONE;

  net_midi_note_subscribe(&(op->midi));
}

// de-init
void op_midi_note_deinit(void* op) {
  // remove from list
  net_midi_note_unsubscribe( &(((op_midi_note_t*)op)->midi) );
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_midi_note_in_chan(op_midi_note_t* op, const io_t v) {
  op->chanIo = v;
  print_dbg("\r\n midi_note, setting channel from input: 0x");
  print_dbg_hex((u32)v);
  op->chan = (s8)(op_to_int(op->chanIo));
  if(op->chan < -1) { op->chan = -1; }
  if(op->chan > 15) { op->chan = 15; }
  print_dbg(" , channel: ");
  print_dbg_hex((u32)(op->chan));
}


static void op_midi_note_handler(op_midi_note_t* op, u8 ch, u8 num, u8 vel) {
  // check status byte
  if(op->chan == -1 || op->chan == ch) {
    net_activate(op, 0, num);
    net_activate(op, 1, vel);
  }
}

static void op_midi_pitch_bend_handler(op_midi_note_t* op, u8 ch, u16 pitch) {
  // check status byte
  if(op->chan == -1 || op->chan == ch) {
    net_activate(op, 2, pitch - MIDI_BEND_ZERO);
  }
}

// pickle / unpickle
u8* op_midi_note_pickle(op_midi_note_t* mnote, u8* dst) {
  dst = pickle_io(mnote->chanIo, dst);
  return dst;
}

const u8* op_midi_note_unpickle(op_midi_note_t* mnote, const u8* src) {
  src = unpickle_io(src, (u32*)&(mnote->chanIo));
  mnote->chan = op_to_int(mnote->chanIo);
  return src;
}
