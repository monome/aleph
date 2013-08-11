#include "net_protected.h"
#include "print_funcs.h"
#include "op_sw.h"

//-------------------------------------------------
//----- descriptor
static const char* op_sw_instring = "VAL     TOG     MUL     ";
static const char* op_sw_outstring = "VAL     ";
static const char* op_sw_opstring = "SW";

//-------------------------------------------------
//----- static function declaration
static void op_sw_inc_func(op_sw_t* sw, const s16 idx, const io_t inc);
static void op_sw_in_state(op_sw_t* sw, const io_t* v);
static void op_sw_in_tog(op_sw_t* sw, const io_t* v);
static void op_sw_in_mul(op_sw_t* sw, const io_t* v);

static op_in_func_t op_sw_in_func[3] = {
  (op_in_func_t)&op_sw_in_state,
  (op_in_func_t)&op_sw_in_tog,
  (op_in_func_t)&op_sw_in_mul
};

//---------------------------------------------
//----- external function definition

/// initialize
void op_sw_init(void* op) {
  op_sw_t* sw = (op_sw_t*)op;
  sw->super.numInputs = 3;
  sw->super.numOutputs = 1;
  sw->outs[0] = -1;
  sw->super.inc_func = (op_inc_func)op_sw_inc_func;
  sw->super.in_func = op_sw_in_func;
  sw->super.in_val = sw->in_val;
  sw->in_val[0] = &(sw->state);
  sw->in_val[1] = &(sw->tog);
  sw->in_val[2] = &(sw->mul);
  sw->super.out = sw->outs;
  sw->super.opString = op_sw_opstring;
  sw->super.inString = op_sw_instring;
  sw->super.outString = op_sw_outstring;
  sw->super.type = eOpSwitch;
  sw->super.flags |= (1 << eOpFlagSys);
  sw->state = 0;
  sw->mul = OP_ONE;
  sw->tog = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_sw_in_state(op_sw_t* sw, const io_t* v) {
  if (sw->tog) {
    // toggle mode, sw state toggles on positive input
    if ( *v > 0) {
      if (sw->state != 0) { 
	sw->state = 0; 
      } else {
	sw->state = sw->mul;
      }
      net_activate(sw->outs[0], sw->state, sw);
    } 
  } else {
    // momentary mode, sw value takes input
    if((*v) > 0) { sw->state = sw->mul; } else { sw->state = 0; }
    net_activate(sw->outs[0], sw->state, sw);
  }
}

// input toggle mode
static void op_sw_in_tog(op_sw_t* sw, const io_t* v) {
  if (*v > 0) { sw->tog = OP_ONE; } else  { sw->tog = 0; } 
}

// input multiplier
static void op_sw_in_mul(op_sw_t* sw, const io_t* v) {
  sw->mul = *v;
  if (sw->state > 0) {
    sw->state = *v;
    net_activate(sw->outs[0], sw->state, sw);
  }
}

//===== UI input

// increment
static void op_sw_inc_func(op_sw_t* sw, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0: // current value
    op_sw_in_state(sw, &inc);
    break;
  case 1: // toggle mode
    op_sw_in_tog(sw, &inc);
    break;
  case 2: // multiplier
    val = OP_ADD(sw->mul, inc);
    op_sw_in_mul(sw, &val);
    break;
  }
}
