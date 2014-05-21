// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_hid8.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_hid8_instring =  "CHAN\0   NUM\0    ";
static const char* op_hid8_outstring = "VAL\0    ";
static const char* op_hid8_opstring = "HID8";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_hid8_in_chan(op_hid8_t* h8, const io_t val);
static void op_hid8_in_num(op_hid8_t* h8, const io_t val);

// pickles
static u8* op_hid8_pickle(op_hid8_t* h8, u8* dst);
static const u8* op_hid8_unpickle(op_hid8_t* h8, const u8* src);

/// hid event handler
static void op_hid8_handler(op_hid_t* op_hid, u32 data);

// input func pointer array
static op_in_fn op_hid8_in_fn[2] = {
  (op_in_fn)&op_hid8_in_chan,
  (op_in_fn)&op_hid8_in_num
};

//-------------------------------------------------
//----- extern function definition
void op_hid8_init(void* mem) {
  //  print_dbg("\r\n op_hid8_init ");
  op_hid8_t* op = (op_hid8_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_hid8_inc_fn;
  op->super.in_fn = op_hid8_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_hid8_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_hid8_unpickle);

  //--- hid
  op->hid.handler = (hid_handler_t)&op_hid8_handler;
  op->hid.sub = op;

  // superclass state

  op->super.type = eOpHid8;
  op->super.flags |= (1 << eOpFlagHid);

  op->super.numInputs = 1;
  op->super.numOutputs = 1;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_hid8_opstring;
  op->super.inString = op_hid8_instring;
  op->super.outString = op_hid8_outstring;

  op->in_val[0] = &(op->byte);
  op->outs[0] = -1;

  op->byte = op_from_int(-1);

  net_hid_list_push(&(op->hid));
}

// de-init
void op_hid8_deinit(void* op) {
  // remove from list
  net_hid_list_remove( &( ((op_hid8_t*)op)->hid) );
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
/* static void op_hid8_in_chan(op_hid8_t* op, const io_t v) { */
/*   // FIXME:: these checks should use io_t specific macros */
/*   if(v < -1) { op->chan = -1;  } */
/*   else if(v > 15) { op->chan = 15; }  */
/*   else { op->chan = v; } */
/* } */

/* static void op_hid8_in_num(op_hid8_t* op, const io_t v) { */
/*   // FIXME:: these checks should use io_t specific macros */
/*   if(v < 0) { op->num = 0; } */
/*   else if (v > 127) { op->num = 127; } */
/*   else { op->num = v; } */
/* } */


static void op_hid8_handler(op_hid_t* op_hid, u32 data) {
  static u8 com;
  static io_t ch, num, val;
  op_hid8_t* op = (op_hid8_t*)(op_hid->sub);

  // check command: status high nib
  /* com = (data & 0xf0000000) >> 28;  */
  /* if (com == 0xb) { // cc */
  /*   if(op->chan < 0) { */
  /*     // take all channels */
  /*     // check number: data 1 */
  /*     num = (data & 0x00ff0000) >> 16; */
  /*     if(num == op->num) { */
  /* 	val = (data & 0x0000ff00) >> 8; */
  /* 	net_activate(op->outs[0], val, op); */
  /*     } */
  /*   } else { */
  /*     // check channel: status low nib */
  /*     ch = (data & 0x0f000000) >> 24; */
  /*     if(ch == op->chan) { */
  /* 	// check number: data 1 */
  /* 	num = (data & 0x00ff0000) >> 16; */
  /* 	if(num == op->num) { */
  /* 	  // send value: data 2 */
  /* 	  val = (data & 0x0000ff00) >> 8; */
  /* 	  net_activate(op->outs[0], val, op); */
  /* 	} */
  /*     } */
  /*   } */
  /* } */
}

// pickle / unpickle
u8* op_hid8_pickle(op_hid8_t* h8, u8* dst) {
  dst = pickle_io(h8->byte, dst);
  return dst;
}

const u8* op_hid8_unpickle(op_hid8_t* h8, const u8* src) {
  src = unpickle_io(src, &(h8->byte));
  //  h8->chan = op_to_int(h8->byte);
  return src;
}
