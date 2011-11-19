/* ctl_op.c
 * aleph-avr32
 *
 * operator implmentation (base and derived classes)
 */

#include <stdio.h>

#include "compiler.h"
#include "ctl_op.h"
#include "ctl_interface.h"

//=========================
// vars
// input and output strings are all the same length, concatenated
// this is lazy, but also efficient.
static const U8 inStringChars = 7;
static const U8 outStringChars = 7;

//=====================================
//===  base class definitions
const U8* ctl_op_getInString(ctl_op_t* op, const U8 idx) {
  return (op->inString + (inStringChars * idx));
}

const U8* ctl_op_getOutString(ctl_op_t* op, const U8 idx) {
  return (op->outString + (outStringChars * idx));
}

//===========================
//=== subclass definitions

//----- switch
static const char* op_sw_instring = "VALUE  TOGGLE ";
static const char* op_sw_outstring = "VALUE ";
static const char* op_sw_opstring = "SWITCH";

static void op_sw_in_val(op_sw_t* sw, const S32* v) {
  if(sw->tog) {
    if (v == 0) {
      return;
    } else {
      sw->val = (sw->val == 0); 
    }
  } else {
    sw->val = (v != 0);
  }
  printf("switch at %d got value input %d\n", (int)sw, *v); 
  if (sw->outs[0] >= 0) {
    ctl_go(sw->outs[0], &(sw->val));
  }
}

static void op_sw_in_tog(op_sw_t* sw, const S32* v) {
  sw->tog = (v != 0);
}

static ctl_in_t op_sw_inputs[2] = {
  (ctl_in_t)&op_sw_in_val,
  (ctl_in_t)&op_sw_in_tog 
};

void op_sw_init(op_sw_t* sw) {
  sw->super.numInputs = 2;
  sw->super.numOutputs = 1;
  sw->outs[0] = -1;
  sw->super.in = op_sw_inputs;
  sw->super.out = sw->outs;
  sw->super.opString = op_sw_opstring;
  sw->super.size = sizeof(op_sw_t);
}

//----- encoder
void op_enc_init(op_enc_t* sw);

//---- adder
void op_add_init(op_add_t* add);

//----- multiplier
void op_mul_init(op_mul_t* mul);

//----- gate
void op_gate_init(op_gate_t* gate);

//------ accumulator
void op_accum_init(op_accum_t* accum);

//------ range selector
void op_sel_init(op_sel_t* sel);

//----- linear map
void op_lin_init(op_lin_t* lin);

//---- param value receiver
void op_param_init(op_param_t* param);

//----- preset manipulator
void op_preset_init(op_preset_t* preset);
