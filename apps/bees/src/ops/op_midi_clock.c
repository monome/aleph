// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_clock.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_clock_instring =  "DUMMY\0  ";
static const char* op_midi_clock_outstring = "TICK\0   START\0  CONT\0   STOP\0   ";
static const char* op_midi_clock_opstring = "MIDICLK";

//-------------------------------------------------
//----- static function declaration

//---- input functions
static void op_midi_clock_in_dummy(op_midi_note_t* op, const io_t v);

//// network inputs: 

// pickles
static u8* op_midi_clock_pickle(op_midi_clock_t* mclock, u8* dst);
static const u8* op_midi_clock_unpickle(op_midi_clock_t* mclock, const u8* src);

/// midi event handler
static void op_midi_clock_clock_tick_handler(op_midi_clock_t *op);
static void op_midi_clock_seq_start_handler(op_midi_clock_t *op);
static void op_midi_clock_seq_stop_handler(op_midi_clock_t *op);
static void op_midi_clock_seq_continue_handler(op_midi_clock_t *op);
// input func pointer array
static op_in_fn op_midi_clock_in_fn[1] = {
  (op_in_fn)&op_midi_clock_in_dummy,
};

//-------------------------------------------------
//----- extern function definition
void op_midi_clock_init(void* mem) {
  //  print_dbg("\r\n op_midi_clock_init ");
  op_midi_clock_t* op = (op_midi_clock_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_midi_clock_inc_fn;
  op->super.in_fn = op_midi_clock_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_midi_clock_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_midi_clock_unpickle);

  //--- midi
  op->midi.handler.clock_tick = (net_midi_real_time_t)&op_midi_clock_clock_tick_handler;
  op->midi.handler.seq_start = (net_midi_real_time_t)&op_midi_clock_seq_start_handler;
  op->midi.handler.seq_stop = (net_midi_real_time_t)&op_midi_clock_seq_stop_handler;
  op->midi.handler.seq_continue = (net_midi_real_time_t)&op_midi_clock_seq_continue_handler;
  op->midi.sub = op;

  // superclass state

  op->super.type = eOpMidiClock;
  op->super.flags |= (1 << eOpFlagMidiIn);

  op->super.numInputs = 1;
  op->super.numOutputs = 4;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_clock_opstring;
  op->super.inString = op_midi_clock_instring;
  op->super.outString = op_midi_clock_outstring;

  op->in_val[0] = &(op->dummy);
  op->outs[0] = 0;
  op->outs[1] = 0;
  op->outs[2] = 0;
  op->outs[3] = 0;

  net_midi_real_time_subscribe(&(op->midi));
}

// de-init
void op_midi_clock_deinit(void* op) {
  // remove from list
  print_dbg("\r\ndeinit...\r\n");
  net_midi_real_time_unsubscribe( &(((op_midi_clock_t*)op)->midi) );
  print_dbg("\r\ndeinit-ed...\r\n");
}

//-------------------------------------------------
//----- static function definition
static void op_midi_clock_in_dummy(op_midi_note_t* op, const io_t v) {
  // nothing to see here...
}
// midi event handler
static void op_midi_clock_clock_tick_handler(op_midi_clock_t *op) {
  net_activate(op, 0, 1);
}
static void op_midi_clock_seq_start_handler(op_midi_clock_t *op) {
  net_activate(op, 1, 1);
}
static void op_midi_clock_seq_continue_handler(op_midi_clock_t *op) {
  net_activate(op, 2, 1);
}
static void op_midi_clock_seq_stop_handler(op_midi_clock_t *op) {
  net_activate(op, 3, 1);
}
// pickle / unpickle
u8* op_midi_clock_pickle(op_midi_clock_t* mclock, u8* dst) {
  return dst;
}

const u8* op_midi_clock_unpickle(op_midi_clock_t* mclock, const u8* src) {
  return src;
}
