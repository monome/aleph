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
#include "op.h"
#include "op_math.h"
#include "op_derived.h"

// operator class registry
// must be laid out identically to eOpId enum!
const op_desc_t op_registry[numOpClasses] = {
{ "SW", sizeof(op_sw_t) },
{ "ENC", sizeof(op_enc_t) },
{ "ADD", sizeof(op_add_t) },
{ "MUL", sizeof(op_mul_t) },
{ "GATE", sizeof(op_gate_t) },
// { "ACCUMULATE", sizeof(op_accum_t) },
// { "SELECT", sizeof(op_sel_t) },
// { "LINEAR MAP", sizeof(op_lin_t) },
// { "PARAMETER", sizeof(op_param_t) },
// { "PRESET", sizeof(op_preset_t) }
};


// input and output strings are all the same length, concatenated
// this is lazy, but also efficient.
static const u8 inStringChars = 8;
static const u8 outStringChars = 8;

//===============================================
//===  base class definittions

// initialize operator at memory
s16 op_init(op_t* op, opId_t opId) {
  // initialize given operator class at memory 
  switch(opId) {
  case eOpSwitch:
    op_sw_init((void*) op);
    break;
  case eOpEnc:
    op_enc_init((void*)op);
    break;
  case eOpAdd:
    op_add_init((void*)op);
    break;
  case eOpMul:
    op_mul_init((void*)op);
    break;
  case eOpGate:
    op_gate_init((void*)op);
    break;
#if 0
  case eOpAccum:
    op_accum_init((void*)op);
    break;
  case eOpSelect:
    return -1;
    break;
  case eOpMapLin:
    return -1;
    break;
#endif
  default:
    return -1;
  }
  op->type = opId;
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
  param_feedback(idx, val);
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
