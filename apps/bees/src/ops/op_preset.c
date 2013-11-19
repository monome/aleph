#include "net_protected.h"
#include "preset.h"
#include "op_preset.h"

// inputs:

// 0 


//-------------------------------------------------
//----- descriptor
static const char* op_preset_instring = "READ    WRITE   IDX     ";
static const char* op_preset_outstring = "IDX     ";
static const char* op_preset_opstring = "PRESET";

//-------------------------------------------------
//----- static function declaration
static void op_preset_inc_fn(op_preset_t* preset, const s16 idx, const io_t inc);
static void op_preset_in_read(op_preset_t* preset, const io_t* v);
static void op_preset_in_write(op_preset_t* preset, const io_t* v);
static void op_preset_idx(op_preset_t* preset, const io_t* v);

static op_in_fn op_preset_in_fn[3] = {
  (op_in_fn) &op_preset_in_read, 
  (op_in_fn) &op_preset_in_write, 
  (op_in_fn) &op_preset_idx,
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

// input read index
static void op_preset_in_read(op_preset_t* preset, const io_t* v) {
  // recall given preset
  preset_recall( OP_TO_INT(*v) );
}

// input write index
static void op_preset_in_write(op_preset_t* preset, const io_t* v) {
  // store given preset
  preset_store( OP_TO_INT(*v) );

}

// input, report last idx (???)
static void op_preset_idx(op_preset_t* preset, const io_t* rw) {
  if(*rw > 0) {
    ///... output
    // preset_last_write();
  } else {
    ///... output
    // preset_last_read();
  }
}

//===== UI input

// increment
static void op_preset_inc_fn(op_preset_t* preset, const s16 idx, const io_t inc) {
  /// FIXME? no meaningful UI
}
