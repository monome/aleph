// asf
#include "print_funcs.h"
// bees
#include "net_protected.h"
#include "op_monome_grid_raw.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_mgrid_raw_instring = "FOCUS   TOG     MONO    ";
static const char* op_mgrid_raw_outstring = "COL     ROW     VAL     ";
static const char* op_mgrid_raw_opstring = "RAWGRID";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_mgrid_raw_inc_func(op_mgrid_raw_t* grid, const s16 idx, const io_t inc);
static void op_mgrid_raw_in_focus(op_mgrid_raw_t* grid, const io_t* val);
static void op_mgrid_raw_in_tog(op_mgrid_raw_t* grid, const io_t* val);
static void op_mgrid_raw_in_mono(op_mgrid_raw_t* grid, const io_t* val);

/// monome event handler
static void op_mgrid_raw_handler(op_mgrid_raw_t* op, u32 edata);

// input func pointer array
static op_in_func_t op_mgrid_raw_in_func[3] = {
  (op_in_func_t)&op_mgrid_raw_in_focus,
  (op_in_func_t)&op_mgrid_raw_in_tog,
  (op_in_func_t)&op_mgrid_raw_in_mono,
};

//-------------------------------------------------
//----- extern function definition
void op_mgrid_raw_init(void* mem) {
  op_mgrid_raw_t* op = (op_mgrid_raw_t*)mem;
  op->super.type = eOpMonomeGridRaw;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 3;
  op->super.numOutputs = 3;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;
  op->super.inc_func = (op_inc_func)op_mgrid_raw_inc_func;
  op->super.in_func = op_mgrid_raw_in_func;

  op->super.opString = op_mgrid_raw_opstring;
  op->super.inString = op_mgrid_raw_instring;
  op->super.outString = op_mgrid_raw_outstring;

  op->in_val[0] = &(op->focus);
  op->in_val[1] = &(op->tog);
  op->in_val[2] = &(op->mono);
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;

  op->monome.handler = &op_mgrid_raw_handler;
  net_monome_set_focus(&(op->monome), 1);

  op->lastPos = 0;
  op->focus = OP_ONE;
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_mgrid_raw_in_focus(op_mgrid_raw_t* op, const io_t* v) {
  op->focus =  ((*v) > 0) * OP_ONE ;
  net_monome_set_focus( &(op->monome), op->focus);
}

static void op_mgrid_raw_in_tog(op_mgrid_raw_t* op, const io_t* v) {
  op->tog  = (*v > 0) * OP_ONE;
}

static void op_mgrid_raw_in_mono(op_mgrid_raw_t* op, const io_t* v) {
  op->mono  = (*v > 0) * OP_ONE;
}

static void op_mgrid_raw_handler(op_mgrid_raw_t* op, u32 edata) {
  static u8 x, y, z;
  static u32 pos;
  static u8 val;

  monome_grid_key_parse_event_data(edata, &x, &y, &z);
  // flat position into led buffer
  pos = x | (y << 4);

  if(op->mono) {
    if(op->tog > 0) { // mono, toggle
      if(z > 0) {        // ignore lift
	val = ( monomeLedBuffer[pos] == 0 );
	monomeLedBuffer[pos] = val;
	if(pos != op->lastPos) {
	  monomeLedBuffer[op->lastPos] = 0;
	}
	print_dbg("\r\n monome grid output...");
	net_activate(op->outs[0], (io_t)x);
	net_activate(op->outs[1], (io_t)y);
	net_activate(op->outs[2], (io_t)val);
	print_dbg("\r\n done.");
      }
    } else { // mono, momentary
      val = z;
      monomeLedBuffer[pos] =  val;
      monomeLedBuffer[op->lastPos] = 0;
      print_dbg("\r\n monome grid output...");
      net_activate(op->outs[0], (io_t)x);
      net_activate(op->outs[1], (io_t)y);
      net_activate(op->outs[2], (io_t)val);
      print_dbg("\r\n done");
    }
  } else {
    if(op->tog > 0) { // poly, toggle
      if(z > 0) {      /// ignore lift
	val = ( monomeLedBuffer[pos] == 0 );
	monomeLedBuffer[pos] = val;
	print_dbg("\r\n monome grid output...");
	net_activate(op->outs[0], (io_t)x);
	net_activate(op->outs[1], (io_t)y);
	net_activate(op->outs[2], (io_t)val);
	print_dbg("\r\n done.");
      }
    } else {   // poly, momentary
      val = z;
      monomeLedBuffer[pos] = val;
      print_dbg("\r\n monome grid output...");
      net_activate(op->outs[0], (io_t)x);
      net_activate(op->outs[1], (io_t)y);
      net_activate(op->outs[2], (io_t)val);
      print_dbg("\r\n done.");
    }
  }
  monome_calc_quadrant_flag(x, y);
  // FIXME: update old pos quadrant too,
  /// need a different calc function since it is flattened array idx, not (x,y)
  op->lastPos = pos;
}


/// increment param value from UI:
static void op_mgrid_raw_inc_func(op_mgrid_raw_t* op, const s16 idx, const io_t inc) {
  switch(idx) {
  case 0: // focus
    op_mgrid_raw_in_focus(op, &inc);
    break;
  case 1: // toggle mode
    op_mgrid_raw_in_tog(op, &inc);
    break;
  case 2: // mono mode
    op_mgrid_raw_in_mono(op, &inc);
    break;
  }
}
