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
static void op_mgrid_raw_handler(op_monome_t* op_monome, u32 edata);

// input func pointer array
static op_in_func_t op_mgrid_raw_in_func[3] = {
  (op_in_func_t)&op_mgrid_raw_in_focus,
  (op_in_func_t)&op_mgrid_raw_in_tog,
  (op_in_func_t)&op_mgrid_raw_in_mono,
};

//-------------------------------------------------
//----- extern function definition
void op_mgrid_raw_init(void* mem) {
  print_dbg("\r\n op_mgrid_raw_init ");

  op_mgrid_raw_t* op = (op_mgrid_raw_t*)mem;
  op->super.type = eOpMonomeGridRaw;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 3;
  op->super.numOutputs = 3;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;
  op->super.inc_func = (op_inc_func)op_mgrid_raw_inc_func;
  op->super.in_func = op_mgrid_raw_in_func;
  //  op->super.deinit = op_mgrid_raw_deinit;

  op->super.opString = op_mgrid_raw_opstring;
  op->super.inString = op_mgrid_raw_instring;
  op->super.outString = op_mgrid_raw_outstring;

  op->in_val[0] = &(op->focus);
  op->in_val[1] = &(op->tog);  
  op->in_val[2] = &(op->mono);
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;

  op->monome.handler = (monome_handler_t)&op_mgrid_raw_handler;
  op->monome.op = op;
  net_monome_set_focus(&(op->monome), 1);

  op->lastPos = 0;
  op->focus = OP_ONE;
}

// de-init
void op_mgrid_raw_deinit(void* op) {
  // release focus
  net_monome_set_focus(&(((op_mgrid_raw_t*)op)->monome), 0);
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_mgrid_raw_in_focus(op_mgrid_raw_t* op, const io_t* v) {
  if((*v) > 0) {
    op->focus = OP_ONE;
  } else {
    op->focus = 0;;
  }
  net_monome_set_focus( &(op->monome), op->focus > 0);
}

static void op_mgrid_raw_in_tog(op_mgrid_raw_t* op, const io_t* v) {
  op->tog  = (*v > 0) ? OP_ONE : 0;
}

static void op_mgrid_raw_in_mono(op_mgrid_raw_t* op, const io_t* v) {
  op->mono  = (*v > 0) ? OP_ONE : 0;
}

static void op_mgrid_raw_handler(op_monome_t* op_monome, u32 edata) {
  static u8 x, y, z;
  static u32 pos;
  static u8 val;

  op_mgrid_raw_t* op = (op_mgrid_raw_t*)(op_monome->op);


  monome_grid_key_parse_event_data(edata, &x, &y, &z);

  /* print_dbg("\r\n op_mgrid_raw_handler received event; x: 0x"); */
  /* print_dbg_hex(x); */
  /* print_dbg("; y: 0x"); */
  /* print_dbg_hex(y); */
  /* print_dbg("; z: 0x"); */
  /* print_dbg_hex(z); */

  // flat position into led buffer
  pos = monome_xy_idx(x, y);

  if(op->mono) {
    if(op->tog > 0) { // mono, toggle
      if(z > 0) {        // ignore lift
	val = ( monomeLedBuffer[pos] == 0 );
	monomeLedBuffer[pos] = val;
	if(pos != op->lastPos) {
	  monomeLedBuffer[op->lastPos] = 0;
	}
	// FIXME: should add macros in op_math.h for io_t conversion
	net_activate(op->outs[0], (io_t)(x<<16), op);
	net_activate(op->outs[1], (io_t)(y<<16), op);
	net_activate(op->outs[2], (io_t)(val<<16), op);
	// refresh flag for current quadrant
	monome_calc_quadrant_flag(x, y);
	// refresh flag for previous quadrant
	monome_idx_xy(op->lastPos, &x, &y);
	monome_calc_quadrant_flag(x, y);
      }
    } else { // mono, momentary
      val = z;
      monomeLedBuffer[pos] =  val;
      monomeLedBuffer[op->lastPos] = 0;
      net_activate(op->outs[0], (io_t)(x<<16), op);
      net_activate(op->outs[1], (io_t)(y<<16), op);
      net_activate(op->outs[2], (io_t)(val<<16), op);
      // refresh flag for current quadrant
      monome_calc_quadrant_flag(x, y);
      // refresh flag for previous quadrant
      monome_idx_xy(op->lastPos, &x, &y);
      monome_calc_quadrant_flag(x, y);  
    }
  } else {
    if(op->tog > 0) { // poly, toggle
      if(z > 0) {      /// ignore lift
	val = ( monomeLedBuffer[pos] == 0 );
	monomeLedBuffer[pos] = val;
	net_activate(op->outs[0], (io_t)(x<<16), op);
	net_activate(op->outs[1], (io_t)(y<<16), op);
	net_activate(op->outs[2], (io_t)(val<<16), op);
	// refresh flag for current quadrant
	monome_calc_quadrant_flag(x, y);
      }
    } else {   // poly, momentary
      val = z;
      monomeLedBuffer[pos] = val;
      net_activate(op->outs[0], (io_t)(x<<16), op);
      net_activate(op->outs[1], (io_t)(y<<16), op);
      net_activate(op->outs[2], (io_t)(val<<16), op);
      // refresh flag for current quadrant
      monome_calc_quadrant_flag(x, y);
    }
  }
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

