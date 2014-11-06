// asf
#include "print_funcs.h"

// avr32 lib
#include "hid.h"

// bees
#include "net_protected.h"
#include "op_hid_word.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_hid_word_instring =  "BYTE\0   SIZE\0   ";
static const char* op_hid_word_outstring = "VAL\0    ";
static const char* op_hid_word_opstring = "HID";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_hid_word_in_byte(op_hid_word_t* h8, const io_t val);
static void op_hid_word_in_size(op_hid_word_t* h8, const io_t val);

// pickles
static u8* op_hid_word_pickle(op_hid_word_t* h8, u8* dst);
static const u8* op_hid_word_unpickle(op_hid_word_t* h8, const u8* src);

/// hid event handler
static void op_hid_word_handler(op_hid_t* op_hid);

// input func pointer array
static op_in_fn op_hid_word_in_fn[2] = {
  (op_in_fn)&op_hid_word_in_byte,
  (op_in_fn)&op_hid_word_in_size,
};

//-------------------------------------------------
//----- extern function definition
void op_hid_word_init(void* mem) {
  //  print_dbg("\r\n op_hid_word_init ");
  op_hid_word_t* op = (op_hid_word_t*)mem;

  // superclass functions
  //--- op
  op->super.in_fn = op_hid_word_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_hid_word_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_hid_word_unpickle);

  //--- hid
  op->hid.handler = (hid_handler_t)&op_hid_word_handler;
  op->hid.sub = op;

  // superclass state

  op->super.type = eOpHid;
  op->super.flags |= (1 << eOpFlagHid);

  op->super.numInputs = 2;
  op->super.numOutputs = 1;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_hid_word_opstring;
  op->super.inString = op_hid_word_instring;
  op->super.outString = op_hid_word_outstring;

  op->in_val[0] = &(op->byte);
  op->in_val[1] = &(op->size);
  op->outs[0] = -1;

  op->byte = op_from_int(0);
  op->size = op_from_int(1);

#if BEEKEEP
#else
  net_hid_list_push(&(op->hid));
#endif
}

// de-init
void op_hid_word_deinit(void* op) {
  // remove from list
#if BEEKEEP
#else
  net_hid_list_remove( &( ((op_hid_word_t*)op)->hid) );
#endif
}

//-------------------------------------------------
//----- static function definition


//--- network input functions

// byte select
static void op_hid_word_in_byte(op_hid_word_t* op, const io_t v) {
#if BEEKEEP
#else
  if(v > HID_FRAME_IDX_MASK) {
    op->byte = HID_FRAME_IDX_MASK;
  } else if (v < 0) { 
    op->byte = 0;
  } else {
    op->byte = v;
  }
#endif
}

// size select
// 0 means single width, 1 means double width
static void op_hid_word_in_size(op_hid_word_t* op, const io_t v) {
  if(v > 0) {
    op->size = OP_ONE;
  } else {
    op->size = 0;
  }
}

// HID frame handler
static void op_hid_word_handler(op_hid_t* op_hid) {
#if BEEKEEP
#else
  op_hid_word_t* op = (op_hid_word_t*)(op_hid->sub);
  const s8* frame;
  //  const u32 dirty;
  const u8 byte = op_to_int(op->byte);
  io_t val;
  // event data is a bitfield indicating which bytes have changed.
  // check bitfield for our byte index
  if(hid_get_byte_flag(byte)) {
    // yes, we actually want to discard volatile here and cast to signed
    frame = (const s8*)hid_get_frame_data();
    if(op->size > 0) {
      /// i have no idea if this is "correct" endianness... 
      /// is that device-specific too?
      val = ( ((io_t)(frame[byte])) << 8 ) | (io_t)( frame[(byte + 1) & HID_FRAME_IDX_MASK] );
    } else {
      /* print_dbg("\r\n op_hid_word: 0x");  */
      /* print_dbg_hex( (int) frame[byte] ); */
      /* print_dbg(" ; 0x");  */
      /* print_dbg_hex( (int) ((io_t) (frame[byte])) ); */
      val = (io_t)(frame[byte]);
    }
    net_activate(op->outs[0], val, op);
  }
#endif
}

// pickle / unpickle
u8* op_hid_word_pickle(op_hid_word_t* h8, u8* dst) {
  dst = pickle_io(h8->byte, dst);
  dst = pickle_io(h8->size, dst);
  return dst;
}

const u8* op_hid_word_unpickle(op_hid_word_t* h8, const u8* src) {
  src = unpickle_io(src, &(h8->byte));
  src = unpickle_io(src, &(h8->size));
  return src;
}
