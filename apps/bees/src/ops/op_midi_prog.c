// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_prog.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_prog_instring =  "CHAN\0   ";
static const char* op_midi_prog_outstring = "VAL\0    ";
static const char* op_midi_prog_opstring = "MIDIPROG";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_midi_prog_in_chan(op_midi_prog_t* mprog, const io_t val);

// pickles
static u8* op_midi_prog_pickle(op_midi_prog_t* mprog, u8* dst);
static const u8* op_midi_prog_unpickle(op_midi_prog_t* mprog, const u8* src);

/// midi event handler
static void op_midi_prog_handler(op_midi_prog_t *op, u8 ch, u8 num);

// input func pointer array
static op_in_fn op_midi_prog_in_fn[1] = {
  (op_in_fn)&op_midi_prog_in_chan
};

//-------------------------------------------------
//----- extern function definition
void op_midi_prog_init(void* mem) {
  //  print_dbg("\r\n op_midi_prog_init ");
  op_midi_prog_t* op = (op_midi_prog_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_midi_prog_inc_fn;
  op->super.in_fn = op_midi_prog_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_midi_prog_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_midi_prog_unpickle);

  //--- midi
  op->midi.handler.program_change = (net_midi_program_change_t)&op_midi_prog_handler;
  op->midi.sub = op;

  // superclass state

  op->super.type = eOpMidiProg;
  op->super.flags |= (1 << eOpFlagMidiIn);

  op->super.numInputs = 1;
  op->super.numOutputs = 1;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_prog_opstring;
  op->super.inString = op_midi_prog_instring;
  op->super.outString = op_midi_prog_outstring;

  op->in_val[0] = &(op->chan);
  op->outs[0] = -1;

  op->chan = op_from_int(-1);

  net_midi_program_change_subscribe(&(op->midi));
}

// de-init
void op_midi_prog_deinit(void* op) {
  // remove from list
  net_midi_program_change_unsubscribe( &(((op_midi_note_t*)op)->midi) );
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_midi_prog_in_chan(op_midi_prog_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < -1) { op->chan = -1;  }
  else if(v > 15) { op->chan = 15; } 
  else { op->chan = v; }
}

// midi event handler
static void op_midi_prog_handler(op_midi_prog_t *op, u8 ch, u8 num) {
  if(op->chan == -1 || op->chan == ch) {
    net_activate(op, 0, num);
  }
}
// pickle / unpickle
u8* op_midi_prog_pickle(op_midi_prog_t* mprog, u8* dst) {
  dst = pickle_io(mprog->chan, dst);
  return dst;
}

const u8* op_midi_prog_unpickle(op_midi_prog_t* mprog, const u8* src) {
  src = unpickle_io(src, (u32*)&(mprog->chan));
  mprog->chan = op_to_int(mprog->chan);

  return src;
}
