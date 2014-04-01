#include "print_funcs.h"

#include "net_protected.h"
#include "preset.h"
#include "op_preset.h"

//-------------------------------------------------
//----- descriptor
static const char* op_preset_instring = "READ\0   WRITE\0  ";
//static const char* op_preset_outstring = "IDX     ";
static const char* op_preset_outstring = "";
static const char* op_preset_opstring = "PRESET";

//-------------------------------------------------
//----- static function declaration
static void op_preset_in_read(op_preset_t* preset, const io_t v);
static void op_preset_in_write(op_preset_t* preset, const io_t v);
//static void op_preset_idx(op_preset_t* preset, const io_t v);

// pickles
static u8* op_preset_pickle	(op_preset_t* preset, u8* dst);
static const u8* op_preset_unpickle(op_preset_t* preset, const u8* src);


static op_in_fn op_preset_in_fn[2] = {
  (op_in_fn) &op_preset_in_read, 
  (op_in_fn) &op_preset_in_write, 
  //  (op_in_fn) &op_preset_idx,
};

//---------------------------------------------
//----- external function definition

/// initialize
void op_preset_init(void* mem) {
  op_preset_t* preset = (op_preset_t*)mem;

  // superclass functions
  preset->super.in_fn = op_preset_in_fn;
  preset->super.pickle = (op_pickle_fn) (&op_preset_pickle);
  preset->super.unpickle = (op_unpickle_fn) (&op_preset_unpickle);

  // superclass state
  preset->super.numInputs = 2;
  preset->super.numOutputs = 0;
  //  preset->super.out = preset->outs;

  //preset->outs[0] = -1;
  preset->in_val[0] = &(preset->read);
  preset->in_val[1] = &(preset->write);
  //  preset->in_val[2] = &(preset->idx);
  
  preset->super.type = eOpPreset;
  preset->super.in_val = preset->in_val;

  preset->super.flags |= (1 << eOpFlagSys);
 
  preset->super.opString = op_preset_opstring;
  preset->super.inString = op_preset_instring;
  preset->super.outString = op_preset_outstring;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input read index
static void op_preset_in_read(op_preset_t* preset, const io_t val) {
  // bounds checks : set, don't perform
  const io_t v = op_to_int(val);
  if(v < 0) { 
    preset->read = 0; 
  } else if (v > (NET_PRESETS_MAX - 1)) { 
    preset->read = op_from_int( NET_PRESETS_MAX - 1); 
  }  else { 
    preset->read = v; 
    preset_recall( v );
  }
}

// input write index
static void op_preset_in_write(op_preset_t* preset, const io_t v) {
  int idx = op_to_int(v);
  preset->write = v;
  // store given preset
  print_dbg("\r\n storing preset from operator, idx: ");
  print_dbg_ulong(idx);
  if(idx >=0 && idx < NET_PRESETS_MAX) { 
    preset_store( idx );
  }
}




// pickles
u8* op_preset_pickle(op_preset_t* preset, u8* dst) {
  //... nothing to do
  return dst;
}

const u8* op_preset_unpickle(op_preset_t* preset, const u8* src) {
  //... nothing to do
  return src;
}
