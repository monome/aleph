

#include "op_monome_grid_raw.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_mgrid_raw_instring = "FOCUS   TOG     MONO    ";
static const char* op_mgrid_raw_outstring = "COL     ROW     VAL     ";
static const char* op_mgrid_raw_opstring = "MGRID_RAW";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_mgrid_raw_inc_func(op_mgrid_raw_t* grid, const s16 idx, const io_t inc);
static void op_mgrid_raw_in_focus(op_mgrid_raw_t* grid, const io_t* val);
static void op_mgrid_raw_in_tog(op_mgrid_raw_t* grid, const io_t* val);
static void op_mgrid_raw_in_mono(op_mgrid_raw_t* grid, const io_t* val);

static op_in_func_t op_mgrid_raw_in_func[2] = {
  (op_in_func_t)&op_mgrid_raw_in_focus,
  (op_in_func_t)&op_mgrid_raw_in_tog,
};

//-------------------------------------------------
//----- extern function definition
void op_mgrid_raw_init(void* mem) {
  op_mgrid_raw_t* op = (op_mgrid_raw_t*)mem;
  op->super.type = eOpMonomeGridRaw;
  op->super.flags |= (1 << eOpFlagMonomeGrid);
  op->super.in_val = op->in_val;
  op->super.out = op->outs;
  op->super.numInputs = 3;
  op->super.numOutputs = 3;
  op->super.inc_func = (op_inc_func)op_mgrid_raw_inc_func;
  op->super.opString = op_mgrid_raw_opstring;
  op->super.inString = op_mgrid_raw_instring;
  op->super.outString = op_mgrid_raw_outstring;
  op->in_val[0] = &(op->focus);
  op->in_val[1] = &(op->tog);
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_mgrid_raw_in_focus(op_mgrid_raw_t* op, const io_t* v) {
}

static void op_mgrid_raw_in_tog(op_mgrid_raw_t* op, const io_t* v) {
  op>tog  = (*v > 0);
}

static void op_mgrid_raw_in_mono(op_mgrid_raw_t* op, const io_t* v) {
  op>tog  = (*v > 0);
}
