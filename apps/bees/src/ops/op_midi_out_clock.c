// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_midi_out_clock.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_midi_out_clock_instring =  "CABLE\0  TICK\0   START\0  CONT\0   STOP\0   ";
static const char* op_midi_out_clock_outstring = "DUMMY\0  ";
static const char* op_midi_out_clock_opstring = "MOUTCK";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_midi_out_clock_in_cable(op_midi_out_clock_t* mout, const io_t val);
static void op_midi_out_clock_in_tick(op_midi_out_clock_t* mout, const io_t val);
static void op_midi_out_clock_in_start(op_midi_out_clock_t* mout, const io_t val);
static void op_midi_out_clock_in_cont(op_midi_out_clock_t* mout, const io_t val);
static void op_midi_out_clock_in_stop(op_midi_out_clock_t* mout, const io_t val);

// pickles
static u8* op_midi_out_clock_pickle(op_midi_out_clock_t* mout, u8* dst);
static const u8* op_midi_out_clock_unpickle(op_midi_out_clock_t* mout, const u8* src);

// input func pointer array
static op_in_fn op_midi_out_clock_in_fn[5] = {
  (op_in_fn)&op_midi_out_clock_in_cable,
  (op_in_fn)&op_midi_out_clock_in_tick,
  (op_in_fn)&op_midi_out_clock_in_start,
  (op_in_fn)&op_midi_out_clock_in_cont,
  (op_in_fn)&op_midi_out_clock_in_stop,
};

//-------------------------------------------------
//----- extern function definition
void op_midi_out_clock_init(void* mem) {
  //  print_dbg("\r\n op_midi_out_clock_init ");
  op_midi_out_clock_t* op = (op_midi_out_clock_t*)mem;

  // superclass functions
  //--- op
  //  op->super.inc_fn = (op_inc_fn)op_midi_out_clock_inc_fn;
  op->super.in_fn = op_midi_out_clock_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_midi_out_clock_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_midi_out_clock_unpickle);

  // superclass state

  op->super.type = eOpMidiOutClock;
  //  op->super.flags |= (1 << eOpFlagMidiIn);

  op->super.numInputs = 5;
  op->super.numOutputs = 1;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_midi_out_clock_opstring;
  op->super.inString = op_midi_out_clock_instring;
  op->super.outString = op_midi_out_clock_outstring;

  op->in_val[0] = &(op->cable);
  op->in_val[1] = &(op->tick);
  op->in_val[2] = &(op->start);
  op->in_val[3] = &(op->cont);
  op->in_val[4] = &(op->stop);
  op->outs[0] = 0;

  op->tick = 0;
  op->start = 0;
  op->cont = 0;
  op->stop = 0;
}

// de-init
void op_midi_out_clock_deinit(void* op) {
  //...
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_midi_out_clock_in_cable(op_midi_out_clock_t* op, const io_t v) {
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

static void op_midi_out_clock_in_tick(op_midi_out_clock_t* mout, const io_t val) {
  u8 pack[3] = {
    0b11111000,
    0,
    0
  };
  midi_write_packet(mout->cable, pack);
}

static void op_midi_out_clock_in_start(op_midi_out_clock_t* mout, const io_t val) {
  u8 pack[3] = {
    0b11111010,
    0,
    0
  };
  midi_write_packet(mout->cable, pack);
}

static void op_midi_out_clock_in_cont(op_midi_out_clock_t* mout, const io_t val) {
  u8 pack[3] = {
    0b11111011,
    0,
    0
  };
  midi_write_packet(mout->cable, pack);
}

static void op_midi_out_clock_in_stop(op_midi_out_clock_t* mout, const io_t val) {
  u8 pack[3] = {
    0b11111100,
    0,
    0
  };
  midi_write_packet(mout->cable, pack);
}

// pickle / unpickle
u8* op_midi_out_clock_pickle(op_midi_out_clock_t* op, u8* dst) {
  dst = pickle_io(op->cable, dst);
  return dst;
}

const u8* op_midi_out_clock_unpickle(op_midi_out_clock_t* op, const u8* src) {
  src = unpickle_io(src, (u32*)&(op->cable));
  return src;
}
