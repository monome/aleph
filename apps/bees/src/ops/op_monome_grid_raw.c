// asf
#include "print_funcs.h"
// bees
#include "net_protected.h"
#include "op_monome_grid_raw.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_mgrid_raw_instring  = "FOCUS\0  X\0      Y\0      VAL\0    ON\0     ";
static const char* op_mgrid_raw_outstring = "X\0      Y\0      BUT\0    ";
static const char* op_mgrid_raw_opstring  = "GRIDRAW";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs:
static void op_mgrid_raw_in_focus(op_mgrid_classic_t* op, const io_t v);
static void op_mgrid_raw_in_x(op_mgrid_raw_t* grid, const io_t val);
static void op_mgrid_raw_in_y(op_mgrid_raw_t* grid, const io_t val);
static void op_mgrid_raw_in_val(op_mgrid_raw_t* grid, const io_t val);
static void op_mgrid_raw_in_on(op_mgrid_raw_t* grid, const io_t val);

// pickles
static u8* op_mgrid_raw_pickle(op_mgrid_raw_t* enc, u8* dst);
static const u8* op_mgrid_raw_unpickle(op_mgrid_raw_t* enc, const u8* src);

/// monome event handler
static void op_mgrid_raw_handler(op_monome_t* op_monome, u32 data);

// input func pointer array
static op_in_fn op_mgrid_raw_in_fn[5] = {
  (op_in_fn)&op_mgrid_raw_in_focus,
  (op_in_fn)&op_mgrid_raw_in_x,
  (op_in_fn)&op_mgrid_raw_in_y,
  (op_in_fn)&op_mgrid_raw_in_val,
  (op_in_fn)&op_mgrid_raw_in_on,
};

//-------------------------------------------------
//----- extern function definition
void op_mgrid_raw_init(void* mem) {
  //  print_dbg("\r\n op_mgrid_raw_init ");
  op_mgrid_raw_t* op = (op_mgrid_raw_t*)mem;

  // superclass functions
  //--- op
  op->super.in_fn = op_mgrid_raw_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_mgrid_raw_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_mgrid_raw_unpickle);

  //--- monome
  op->monome.handler = (monome_handler_t)&op_mgrid_raw_handler;
  net_monome_init(&op->monome, op);

  // superclass state

  op->super.type = eOpMonomeGridRaw;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 5;
  op->super.numOutputs = 3;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_mgrid_raw_opstring;
  op->super.inString = op_mgrid_raw_instring;
  op->super.outString = op_mgrid_raw_outstring;

  op->in_val[0] = &(op->focus);
  op->monome.focus = &(op->focus);
  op->in_val[1] = &(op->x);
  op->in_val[2] = &(op->y);
  op->in_val[3] = &(op->ledVal);
  op->in_val[4] = &(op->ledOn);
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;

  net_monome_set_focus(&(op->monome), 1);
}

// de-init
void op_mgrid_raw_deinit(void* op) {
  // release focus
  net_monome_set_focus(&(((op_mgrid_raw_t*)op)->monome), 0);
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_mgrid_raw_in_focus(op_mgrid_classic_t* op, const io_t v) {
  if((v) > 0) {
    op->focus = OP_ONE;
  } else {
    // if(op->focus > 0) { net_monome_grid_clear(); }
    op->focus = 0;
  }
  net_monome_set_focus( &(op->monome), op->focus > 0);
}

static void op_mgrid_raw_in_x(op_mgrid_raw_t* op, const io_t v) {
  if(v > 15)
    op->x = 15;
  else if (v < 0)
    op->x = 0;
  else
    op->x = v;
}

static void op_mgrid_raw_in_y(op_mgrid_raw_t* op, const io_t v) {
  if(v > 15)
    op->y = 15;
  else if (v < 0)
    op->y = 0;
  else
    op->y = v;
}

static void op_mgrid_raw_in_val(op_mgrid_raw_t* op, io_t v) {
  if(v > 15) {
    op->ledVal = 15;
    op->ledOn = 1;
  }
  else if (v <= 0) {
    op->ledVal = 0;
    op->ledOn = 0;
  }
  else {
    op->ledVal = v;
    op->ledOn = 1;
  }
  op->monome.opLedBuffer[monome_xy_idx(op->x, op->y)]= op->ledVal;
  if(op->focus > 0) {
    monome_calc_quadrant_flag(op->x, op->y);
  }
}

static void op_mgrid_raw_in_on(op_mgrid_raw_t* op, io_t v) {
  if(v <= 0) {
    op->ledVal = 0;
    op->ledOn = 0;
  }
  else {
    op->ledVal = 15;
    op->ledOn = 1;
  }
  op->monome.opLedBuffer[monome_xy_idx(op->x, op->y)]= op->ledVal;
  if(op->focus > 0) {
    monome_calc_quadrant_flag(op->x, op->y);
  }
}

static void op_mgrid_raw_handler(op_monome_t* op_monome, u32 edata) {
  static u8 x, y, z;

  op_mgrid_raw_t* op = (op_mgrid_raw_t*)(op_monome->op);

  monome_grid_key_parse_event_data(edata, &x, &y, &z);
  net_activate(op, 0, x);
  net_activate(op, 1, y);
  net_activate(op, 2, z);
}



// pickle / unpickle
u8* op_mgrid_raw_pickle(op_mgrid_raw_t* mgrid, u8* dst) {
  dst = pickle_io(mgrid->focus, dst);
  dst = pickle_io(mgrid->x, dst);
  dst = pickle_io(mgrid->y, dst);
  return dst;
}

const u8* op_mgrid_raw_unpickle(op_mgrid_raw_t* mgrid, const u8* src) {
  src = unpickle_io(src, (u32*)&(mgrid->focus));
  src = unpickle_io(src, (u32*)&(mgrid->x));
  src = unpickle_io(src, (u32*)&(mgrid->y));
  return src;
}
