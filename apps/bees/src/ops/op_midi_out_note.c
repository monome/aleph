// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_out_note.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_out_note_instring =  "CHAN\0   NUM\0    VEL\0    ";
static const char* op_midi_out_note_outstring = "";
static const char* op_midi_out_note_opstring = "MOUT_N";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_midi_out_note_in_chan(op_midi_out_note_t* mout, const io_t val);
static void op_midi_out_note_in_vel(op_midi_out_note_t* mout, const io_t val);
static void op_midi_out_note_in_num(op_midi_out_note_t* mout, const io_t val);

// pickles
static u8* op_midi_out_note_pickle(op_midi_out_note_t* mout, u8* dst);
static const u8* op_midi_out_note_unpickle(op_midi_out_note_t* mout, const u8* src);

// build and send a midi serial packet
static void op_midi_out_note_send_packet( op_midi_out_note_t* mout );

// input func pointer array
static op_in_fn op_midi_out_note_in_fn[3] = {
  (op_in_fn)&op_midi_out_note_in_chan,
  (op_in_fn)&op_midi_out_note_in_num,
  (op_in_fn)&op_midi_out_note_in_vel,
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

  op->super.numInputs = 3;
  op->super.numOutputs = 0;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_out_note_opstring;
  op->super.inString = op_midi_out_note_instring;
  op->super.outString = op_midi_out_note_outstring;

  op->in_val[0] = &(op->chan);
  op->in_val[1] = &(op->num);
  op->in_val[2] = &(op->vel);

  op->chan = OP_ONE;
  op->num = 0;
  op->vel = 0;

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

/*
static void op_midi_out_note_handler(op_midi_t* op_midi, u32 data) {
  static u8 com;
  static io_t ch, num, val;
  op_midi_out_note_t* op = (op_midi_out_note_t*)(op_midi->sub);

  // check command: status high nib
  com = (data & 0xf0000000) >> 28; 
  if (com == 0xb) { // out_note
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
*/

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

  print_dbg("\r\n midi_out_note_send_packet; data: ");
  print_dbg_char_hex(pack[0]);    print_dbg(" ");
  print_dbg_char_hex(pack[1]);    print_dbg(" ");
  print_dbg_char_hex(pack[2]);    print_dbg(" ");

  midi_write(pack, 3);

}

// pickle / unpickle
u8* op_midi_out_note_pickle(op_midi_out_note_t* mout, u8* dst) {
  dst = pickle_io(mout->chan, dst);
  return dst;
}

const u8* op_midi_out_note_unpickle(op_midi_out_note_t* mout, const u8* src) {
  src = unpickle_io(src, (u32*)&(mout->chan));
  mout->chan = op_to_int(mout->chan);
  return src;
}
