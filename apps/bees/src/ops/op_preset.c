#include "net_protected.h"
//#include "print_fns.h"
#include "op_preset.h"

// inputs:

// 0 


//-------------------------------------------------
//----- descriptor
static const char* op_preset_instring = "READ    WRITE   REREAD  REWRITE  GETIDX  ";
static const char* op_preset_outstring = "IDX     ";
static const char* op_preset_opstring = "PRESET";

//-------------------------------------------------
//----- static function declaration
static void op_preset_inc_fn(op_preset_t* preset, const s16 idx, const io_t inc);
static void op_preset_in_read(op_preset_t* preset, const io_t* v);
static void op_preset_in_write(op_preset_t* preset, const io_t* v);
static void op_preset_in_idx(op_preset_t* preset, const io_t* v);

static op_in_fn op_preset_in_fn[3] = {
  (op_in_fn) &op_preset_in_read, 
  (op_in_fn) &op_preset_in_write, 
  (op_in_fn) &op_preset_in_idx,
};

//---------------------------------------------
//----- external function definition

/// initialize
void op_preset_init(void* mem) {
  op_preset_t* preset = (op_preset_t*)mem;
  preset->super.numInputs = 3;
  preset->super.numOutputs = 1;
  preset->outs[0] = -1;
  preset->super.inc_fn = (op_inc_fn)op_preset_inc_fn;
  preset->super.in_fn = op_preset_in_fn;
  preset->super.in_val = preset->in_val;
  preset->in_val[0] = &(preset->read);
  preset->in_val[1] = &(preset->write);
  preset->in_val[2] = &(preset->idx);
  preset->super.out = preset->outs;
  preset->super.opString = op_preset_opstring;
  preset->super.inString = op_preset_instring;
  preset->super.outString = op_preset_outstring;
  preset->super.type = eOpSwitch;
  preset->super.flags |= (1 << eOpFlagSys);
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_preset_in_read(op_preset_t* preset, const io_t* v) {
  // recall given preset
}

// input toggle mode
static void op_preset_in_write(op_preset_t* preset, const io_t* v) {
  // store given preset

}

// input multiplier
static void op_preset_in_idx(op_preset_t* preset, const io_t* v) {
  // report the last used index
  //...
  //  net_activate
}

//===== UI input

// increment
static void op_preset_inc_fn(op_preset_t* preset, const s16 idx, const io_t inc) {
  /// no meaningful UI

  /* io_t val; */
  /* switch(idx) { */
  /* case 0: // current value */
  /*   break; */
  /* case 1: // toggle mode */
  /*   break; */
  /* case 2: // multiplier */
  /*   op_preset_in_mul(sw, &val); */
  /*   break; */
  /* } */
}
