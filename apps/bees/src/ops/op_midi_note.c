// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_note.h"


//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_note_instring = "CHAN\0   ";
static const char* op_midi_note_outstring = "NUM\0    VELON\0  VELOFF\0  ";
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
static void op_midi_note_handler(op_midi_t* op_midi, u32 data);

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
  op->midi.handler = (midi_handler_t)&op_midi_note_handler;
  op->midi.sub = op;

  // superclass state

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
  op->outs[1] = -1;

  op->chan = -1;
  op->chanIo = OP_NEG_ONE;

  // FIXME: should sanity-check that the op isn't already in the dang list
  net_midi_list_push(&(op->midi));
}

// de-init
void op_midi_note_deinit(void* op) {
  // remove from list
  net_midi_list_remove( &(((op_midi_note_t*)op)->midi) );
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


static void op_midi_note_handler(op_midi_t* op_midi, u32 data) {
  static u8 com;
  static u8 ch, num, vel;
  op_midi_note_t* op = (op_midi_note_t*)(op_midi->sub);

  // check status byte  
  com = (data & 0xf0000000) >> 28; 
  if (com == 0x9) {

    if(op->chan != -1) {
      ch = (data & 0x0f000000) >> 24;
      if(ch != op->chan) {
	return;
      }
    }
    
    // got here, so channel matched, or we want all channels
    num = (data & 0xff0000) >> 16;
    vel = (data & 0xff00) >> 8;
    net_activate(op->outs[0], op_from_int(num), op);
    net_activate(op->outs[1], op_from_int(vel), op);
    
  } else if (com == 0x8) {
    // note off
    if(op->chan != -1) {
      ch = (data & 0x0f000000) >> 24;
      if(ch != op->chan) {
	return;
      }
    }
    // got here, so channel matched, or we want all channels
    num = (data & 0xff0000) >> 16;
    vel = (data & 0xff00) >> 8;
    net_activate(op->outs[0], op_from_int(num), op);
    net_activate(op->outs[1], 0, op);
    net_activate(op->outs[2], op_from_int(vel), op);
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
