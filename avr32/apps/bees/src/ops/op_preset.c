#include "net_protected.h"
//#include "print_funcs.h"
#include "op_preset.h"

// inputs:

// 0 


//-------------------------------------------------
//----- descriptor
static const char* op_preset_instring = "READ    WRITE   REREAD  REWRITE  GETIDX  "
static const char* op_preset_outstring = "IDX     ";
static const char* op_preset_opstring = "PRESET";

//-------------------------------------------------
//----- static function declaration
static void op_preset_inc_func(op_preset_t* preset, const s16 idx, const io_t inc);
static void op_preset_in_read(op_preset_t* preset, const io_t* v);
static void op_preset_in_write(op_preset_t* preset, const io_t* v);
static void op_preset_in_reread(op_preset_t* preset, const io_t* v);
static void op_preset_in_rewrite(op_preset_t* preset, const io_t* v);
static void op_preset_in_getidx(op_preset_t* preset, const io_t* v);

static op_in_func_t op_preset_in_func[3] = {
  (op_in_func_t) &op_preset_in_read, 
  (op_in_func_t) &op_preset_in_write, 
  (op_in_func_t) &op_preset_in_reread, 
  (op_in_func_t) &op_preset_in_rewrite, 
  (op_in_func_t) &op_preset_in_getidx, 
};

//---------------------------------------------
//----- external function definition

/// initialize
void op_preset_init(op_preset_t* preset) {
  preset->super.numInputs = 5;
  preset->super.numOutputs = 1;
  preset->outs[0] = -1;
  preset->super.inc_func = (op_inc_func)op_preset_inc_func;
  preset->super.in_func = op_preset_in_func;
  preset->super.in_val = preset->in_val;
  preset->in_val[0] = &(preset->state);
  preset->in_val[1] = &(preset->tog);
  preset->in_val[2] = &(preset->mul);
  preset->super.out = preset->outs;
  preset->super.opString = op_preset_opstring;
  preset->super.inString = op_preset_instring;
  preset->super.outString = op_preset_outstring;
  preset->super.type = eOpSwitch;
  preset->super.flags |= (1 << eOpFlagSys);
  preset->state = 0;
  preset->mul = OP_ONE;
  preset->tog = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_preset_in_state(op_preset_t* preset, const io_t* v) {
  if (preset->tog) {
    // toggle mode, sw state toggles on positive input
    if ( *v > 0) {
      if (preset->state != 0) { 
	preset->state = 0; 
      } else {
	preset->state = preset->mul;
      }
      net_activate(preset->outs[0], preset->state);
    } 
  } else {
    // momentary mode, sw value takes input
    //    preset->state = (((*v) > 0) ? preset->mul : 0);
    if((*v) > 0) { preset->state = preset->mul; } else { preset->state = 0; }
    net_activate(preset->outs[0], preset->state);
  }
}

// input toggle mode
static void op_preset_in_tog(op_preset_t* preset, const io_t* v) {
  //  preset->tog = (io_t)(*v > 0);
  if (*v > 0) { preset->tog = OP_ONE; } else  { preset->tog = 0; } 
  /*
  if (preset-> val > 0) { preset->state = 0; }
  else { preset->state = preset->mul; }
  net_activate(preset->outs[0], preset->state);
  */
}

// input multiplier
static void op_preset_in_mul(op_preset_t* preset, const io_t* v) {
  preset->mul = *v;
  if (preset->state > 0) {
    preset->state = *v;
    net_activate(preset->outs[0], preset->state);
  }
}

//===== UI input

// increment
static void op_preset_inc_func(op_preset_t* preset, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0: // current value
    op_preset_in_state(sw, &inc);
    break;
  case 1: // toggle mode
    op_preset_in_tog(sw, &inc);
    break;
  case 2: // multiplier
    val = OP_ADD(preset->mul, inc);
    /* print_dbg("\r\n sw/mul increment; val: "); */
    /* print_dbg_hex(val); */
    op_preset_in_mul(sw, &val);
    break;
  }
}
