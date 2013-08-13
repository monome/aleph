/* ctl_op.c
 * aleph-avr32
 *
 * operator implmentation (base and derived classes)
 */

// std
//#include <stdio.h>
// asf
#ifdef ARCH_AVR32
#include "print_funcs.h"
#endif
// aleph
#include "types.h"
#include "menu_protected.h"
#include "net.h"
#include "param.h"
#include "play.h"
#include "op.h"
#include "op_math.h"
#include "op_derived.h"

// operator class registry
// must be laid out identically to eOpId enum!
const op_desc_t op_registry[numOpClasses] = {
  {
    .name = "SW",
    .size = sizeof(op_sw_t), 
    .init = &op_sw_init, 
    .deinit = NULL
  }, {
    .name = "ENC", 
    .size = sizeof(op_enc_t),
    .init = &op_enc_init,
    .deinit = NULL
  }, {
    .name = "ADD",
    .size = sizeof(op_add_t) ,
    .init = &op_add_init,
    .deinit = NULL
  } , {
    .name = "MUL",
    .size = sizeof(op_mul_t) ,
    .init = &op_mul_init,
    .deinit = NULL
  } , {
    .name = "GATE",
    .size = sizeof(op_gate_t), 
    .init = &op_gate_init,
    .deinit = NULL
  } , {
    .name = "RAWGRID",
    .size = sizeof(op_mgrid_raw_t),
    .init = &op_mgrid_raw_init,
    .deinit = &op_mgrid_raw_deinit
  }
  // { "ACCUMULATE", sizeof(op_accum_t) },
  // { "SELECT", sizeof(op_sel_t) },
  // { "LINEAR MAP", sizeof(op_lin_t) },
  // { "PARAMETER", sizeof(op_param_t) }, q
  // { "PRESET", sizeof(op_preset_t) }
};


// input and output strings are all the same length, concatenated
// this is lazy, but also efficient.
static const u8 inStringChars = 8;
static const u8 outStringChars = 8;

//===============================================
//===  base class definittions

// initialize operator at memory
s16 op_init(op_t* op, op_id_t opId) {
  // no flags by default
  op->flags = 0x00000000;
  // run class init function
  (*(op_registry[opId].init))(op);
  return 0;
}

// de-initialize operator
s16 op_deinit(op_t* op) {
  op_class_deinit_t f = op_registry[op->type].deinit;
  
  if(f != NULL) {
    print_dbg("\r\n de-initializing operator at address 0x");
    print_dbg_hex((u32)op);
    (*f)(op);
  }
  return 0;
}



const char* op_in_name(op_t* op, const u8 idx) {
  static char str[16];
  u8 i;
  // str = (op->inString + (inStringChars * idx));
  for(i=0; i<inStringChars; i++) {
    str[i] = *(op->inString + (inStringChars * idx) + i);
  }
  str[inStringChars] = '\0';
  return str;
}

const char* op_out_name(op_t* op, const u8 idx) {
  static char str[16];
  u8 i;
  for(i=0; i<outStringChars; i++) {
    str[i] = *(op->outString + (outStringChars * idx) + i);
  }
  str[outStringChars] = '\0';
  return str;
}

// get input value
io_t op_get_in_val(op_t* op, s16 idx) {
  return *(op->in_val[idx]);
}

// set input value
void op_set_in_val(op_t* op, s16 idx, io_t val) {
  io_t * const pIn = (op->in_val[idx]);
  *pIn = val;
  (*(op->in_func[idx]))(op, pIn);  
  //  param_feedback(idx, val);
  //  play_input(idx);
  // TODO: check for play flag
}

//============================================
//=== subclass definitions


//-------------------------------------------------
//------ range selector
//void op_sel_init(op_sel_t* sel);

//-------------------------------------------------
//----- linear map
//void op_lin_init(op_lin_t* lin);

//-------------------------------------------------
//---- param value receiver
/*
static const char* op_param_instring = "INDEX   VALUE   ";
static const char* op_param_opxstring = "PARAM";

static void op_param_in_idx(op_param_t* param, const io_t* v) {
  // FIXME: limit by DSP param count
  param->idx = (u32)(*v);
}

static void op_param_in_val(op_param_t* param, const io_t* v) {
  // FIXME: set DSP param
 
}

static op_in_func_t op_param_inputs[2] = {
  (op_in_func_t)&op_param_in_idx,
  (op_in_func_t)&op_param_in_val
};

void op_param_init(op_param_t* op) {
  op->super.numInputs = 2;
  op->super.numOutputs = 0;
  op->super.in_func = op_param_inputs;
  op->super.opString = op_param_opstring;
  op->super.inString = op_param_instring;
  op->super.type = eOpParam;  
  op->super.status = eSysRecOp;  
}

*/

//-------------------------------------------------
//----- preset manipulator
//void op_preset_init(op_preset_t* preset);
