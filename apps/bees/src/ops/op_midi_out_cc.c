// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_out_cc.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_out_cc_instring =  "CHAN\0   NUM\0    VAL\0    ";
static const char* op_midi_out_cc_outstring = "";
static const char* op_midi_out_cc_opstring = "MOUT_CC";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_midi_out_cc_in_chan(op_midi_out_cc_t* mout, const io_t val);
static void op_midi_out_cc_in_val(op_midi_out_cc_t* mout, const io_t val);
static void op_midi_out_cc_in_num(op_midi_out_cc_t* mout, const io_t val);

// pickles
static u8* op_midi_out_cc_pickle(op_midi_out_cc_t* mout, u8* dst);
static const u8* op_midi_out_cc_unpickle(op_midi_out_cc_t* mout, const u8* src);

// build and send a midi serial packet
static void op_midi_out_cc_send_packet( op_midi_out_cc_t* mout );

// input func pointer array
static op_in_fn op_midi_out_cc_in_fn[3] = {
  (op_in_fn)&op_midi_out_cc_in_chan,
  (op_in_fn)&op_midi_out_cc_in_num,
  (op_in_fn)&op_midi_out_cc_in_val,
};

//-------------------------------------------------
//----- extern function definition
void op_midi_out_cc_init(void* mem) {
  //  print_dbg("\r\n op_midi_out_cc_init ");
  op_midi_out_cc_t* op = (op_midi_out_cc_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_midi_out_cc_inc_fn;
  op->super.in_fn = op_midi_out_cc_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_midi_out_cc_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_midi_out_cc_unpickle);

  // superclass state

  op->super.type = eOpMidiOutCC;
  //  op->super.flags |= (1 << eOpFlagMidiIn);

  op->super.numInputs = 3;
  op->super.numOutputs = 0;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_out_cc_opstring;
  op->super.inString = op_midi_out_cc_instring;
  op->super.outString = op_midi_out_cc_outstring;

  op->in_val[0] = &(op->chan);
  op->in_val[1] = &(op->num);
  op->in_val[2] = &(op->val);

  op->chan = OP_ONE;
  op->num = 0;
  op->val = 0;

}

// de-init
void op_midi_out_cc_deinit(void* op) {
  //...
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_midi_out_cc_in_chan(op_midi_out_cc_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < -1) { op->chan = -1;  }
  else if(v > 15) { op->chan = 15; } 
  else { op->chan = v; }
}


static void op_midi_out_cc_in_num(op_midi_out_cc_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < 0) { op->num = 0; }
  else if (v > 127) { op->num = 127; }
  else { 
    op->num = v;
  }
}

static void op_midi_out_cc_in_val(op_midi_out_cc_t* op, const io_t v) {
  // FIXME:: these checks should use io_t specific macros
  if(v < 0) { op->val = 0; }
  else if (v > 127) { op->val = 127; }
  else { op->val = v; }

  op_midi_out_cc_send_packet(op);
}

// build and send a midi serial packet
void op_midi_out_cc_send_packet( op_midi_out_cc_t* mout ) {
  u8 pack[3];
  // control change: high nib = 1011 
  pack[0] = 0xb0;
  // low nib = channel
  pack[0] |= (u8)(mout->chan & 0x0f);
  // two data bytes: number, value
  pack[1] = (u8)(mout->num);
  pack[2] = (u8)(mout->val);

  /* print_dbg("\r\n midi_out_cc_send_packet; data: "); */
  /* print_dbg_char_hex(pack[0]);    print_dbg(" "); */
  /* print_dbg_char_hex(pack[1]);    print_dbg(" "); */
  /* print_dbg_char_hex(pack[2]);    print_dbg(" "); */

  midi_write(pack, 3);

}

// pickle / unpickle
u8* op_midi_out_cc_pickle(op_midi_out_cc_t* op, u8* dst) {
  dst = pickle_io(op->chan, dst);
  dst = pickle_io(op->num, dst);
  return dst;
}

const u8* op_midi_out_cc_unpickle(op_midi_out_cc_t* op, const u8* src) {
  src = unpickle_io(src, (u32*)&(op->chan));
  src = unpickle_io(src, (u32*)&(op->num));
  op->chan = op_to_int(op->chan);
  op->num = op_to_int(op->num);
  return src;
}
