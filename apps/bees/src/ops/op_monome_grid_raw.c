// asf
#include "print_funcs.h"
// bees
#include "net_protected.h"
#include "op_monome_grid_raw.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_mgrid_raw_instring  = "X\0      Y\0      LED\0    ";
static const char* op_mgrid_raw_outstring = "X\0      Y\0      BUT\0    ";
static const char* op_mgrid_raw_opstring  = "GRID";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_mgrid_raw_in_x(op_mgrid_raw_t* grid, const io_t val);
static void op_mgrid_raw_in_y(op_mgrid_raw_t* grid, const io_t val);
static void op_mgrid_raw_in_led(op_mgrid_raw_t* grid, const io_t val);

// pickles
static u8* op_mgrid_raw_pickle(op_mgrid_raw_t* enc, u8* dst);
static const u8* op_mgrid_raw_unpickle(op_mgrid_raw_t* enc, const u8* src);

/// monome event handler
static void op_mgrid_raw_handler(op_monome_t* op_monome, u32 data);

// input func pointer array
static op_in_fn op_mgrid_raw_in_fn[3] = {
  (op_in_fn)&op_mgrid_raw_in_x,
  (op_in_fn)&op_mgrid_raw_in_y,
  (op_in_fn)&op_mgrid_raw_in_led,
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
  op->monome.op = op;

  // superclass state

  op->super.type = eOpMonomeGridRaw;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 3;
  op->super.numOutputs = 3;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_mgrid_raw_opstring;
  op->super.inString = op_mgrid_raw_instring;
  op->super.outString = op_mgrid_raw_outstring;

  op->in_val[0] = &(op->x);
  op->in_val[1] = &(op->y);
  op->in_val[2] = &(op->ledState);
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
static void op_mgrid_raw_in_x(op_mgrid_raw_t* op, const io_t v) {
  if (v < 16 && v >= 0)
    op->x = v;
}

static void op_mgrid_raw_in_y(op_mgrid_raw_t* op, const io_t v) {
  if (v < 16 && v >= 0)
    op->y = v;
}

static void op_mgrid_raw_in_led(op_mgrid_raw_t* op, const io_t v) {
  if (v < 16 && v >= 0) {
    monome_grid_led_set(op->x, op->y, v);
    op->ledState = v;
  }
}

static void op_mgrid_raw_handler(op_monome_t* op_monome, u32 edata) {
  static u8 x, y, z;

  op_mgrid_raw_t* op = (op_mgrid_raw_t*)(op_monome->op);

  monome_grid_key_parse_event_data(edata, &x, &y, &z);
  net_activate(op->outs[0], x, op);
  net_activate(op->outs[1], y, op);
  net_activate(op->outs[2], z, op);
}



// pickle / unpickle
u8* op_mgrid_raw_pickle(op_mgrid_raw_t* mgrid, u8* dst) {
  dst = pickle_io(mgrid->x, dst);
  dst = pickle_io(mgrid->y, dst);
  return dst;
}

const u8* op_mgrid_raw_unpickle(op_mgrid_raw_t* mgrid, const u8* src) {
  src = unpickle_io(src, (u32*)&(mgrid->x));
  src = unpickle_io(src, (u32*)&(mgrid->y));
  return src;
}
