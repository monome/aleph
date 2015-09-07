// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_cc.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_cc_instring =  "CHAN\0   NUM\0    ";
static const char* op_midi_cc_outstring = "VAL\0    ";
static const char* op_midi_cc_opstring = "MIDICC";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_midi_cc_in_chan(op_midi_cc_t* mcc, const io_t val);
static void op_midi_cc_in_num(op_midi_cc_t* mcc, const io_t val);

// pickles
static u8* op_midi_cc_pickle(op_midi_cc_t* mcc, u8* dst);
static const u8* op_midi_cc_unpickle(op_midi_cc_t* mcc, const u8* src);

/// midi event handler
static void op_midi_cc_handler(op_midi_t* op_midi, u32 data);

// input func pointer array
static op_in_fn op_midi_cc_in_fn[2] = {
  (op_in_fn)&op_midi_cc_in_chan,
  (op_in_fn)&op_midi_cc_in_num
};

//-------------------------------------------------
//----- extern function definition
void op_midi_cc_init(void* mem) {
  //  print_dbg("\r\n op_midi_cc_init ");
  op_midi_cc_t* op = (op_midi_cc_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_midi_cc_inc_fn;
  op->super.in_fn = op_midi_cc_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_midi_cc_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_midi_cc_unpickle);

  //--- midi
  op->midi.handler = (midi_handler_t)&op_midi_cc_handler;
  op->midi.sub = op;

  // superclass state

  op->super.type = eOpMidiCC;
  op->super.flags |= (1 << eOpFlagMidiIn);

  op->super.numInputs = 2;
  op->super.numOutputs = 1;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_cc_opstring;
  op->super.inString = op_midi_cc_instring;
  op->super.outString = op_midi_cc_outstring;

  op->in_val[0] = &(op->chan);
  op->in_val[1] = &(op->num);
  op->outs[0] = -1;

  op->chan = op_from_int(-1);
  op->num = 0;

  net_midi_list_push(&(op->midi));
}

// de-init
void op_midi_cc_deinit(void* op) {
  // remove from list
  net_midi_list_remove( &(((op_midi_note_t*)op)->midi) );
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_midi_cc_in_chan(op_midi_cc_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < -1) { op->chan = -1;  }
  else if(v > 15) { op->chan = 15; } 
  else { op->chan = v; }
}

static void op_midi_cc_in_num(op_midi_cc_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < 0) { op->num = 0; }
  else if (v > 127) { op->num = 127; }
  else { op->num = v; }
}

// midi event handler
static void op_midi_cc_handler(op_midi_t* op_midi, u32 data) {
  static u8 com;
  static io_t ch, num, val;
  op_midi_cc_t* op = (op_midi_cc_t*)(op_midi->sub);

  // check command: status high nib
  com = (data & 0xf0000000) >> 28; 
  if (com == 0xb) { // cc
    if(op->chan < 0) {
      // take all channels
      // check number: data 1
      num = (data & 0x00ff0000) >> 16;
      if(num == op->num) {
	val = (data & 0x0000ff00) >> 8;
	net_activate(op->outs[0], val, op);
      }
    } else {
      // check channel: status low nib
      ch = (data & 0x0f000000) >> 24;
      if(ch == op->chan) {
	// check number: data 1
	num = (data & 0x00ff0000) >> 16;
	if(num == op->num) {
	  // send value: data 2
	  val = (data & 0x0000ff00) >> 8;
	  net_activate(op->outs[0], val, op);
	}
      }
    }
  }
}

// pickle / unpickle
u8* op_midi_cc_pickle(op_midi_cc_t* mcc, u8* dst) {
  dst = pickle_io(mcc->chan, dst);
  dst = pickle_io(mcc->num, dst);
  return dst;
}

const u8* op_midi_cc_unpickle(op_midi_cc_t* mcc, const u8* src) {
  src = unpickle_io(src, (u32*)&(mcc->chan));
  mcc->chan = op_to_int(mcc->chan);
  src = unpickle_io(src, (u32*)&(mcc->num));
  mcc->num = op_to_int(mcc->num);

  return src;
}
