/* ctl_op.c
 * aleph-avr32
 *
 * operator implmentation (base and derived classes)
 */

#include "compiler.h"
#include "ctl_op.h"
#include "ctl_interface.h"

#include <stdio.h>

//=========================
// vars

// operator class registry
// must be laid out idenitcally to eOpId enum
op_desc_t op_registry[numOpClasses] = {
  { "SWITCH", sizeof(op_sw_t) },
  { "ENCODER", sizeof(op_enc_t) },
  { "ADD", sizeof(op_add_t) },
  { "MULTIPLY", sizeof(op_mul_t) },
  { "GATE", sizeof(op_gate_t) },
  { "ACCUMULATE", sizeof(op_accum_t) },
  { "SELECT", sizeof(op_sel_t) },
  { "LINEAR MAP", sizeof(op_lin_t) },
  { "PARAMETER", sizeof(op_param_t) },
  { "PRESET", sizeof(op_preset_t) }
};

// input and output strings are all the same length, concatenated
// this is lazy, but also efficient.
static const U8 inStringChars = 8;
static const U8 outStringChars = 8;

//=====================================
//===  base class definitions
const char* ctl_op_in_name(ctl_op_t* op, const U8 idx) {
  return (op->inString + (inStringChars * idx));
}

const char* ctl_op_out_name(ctl_op_t* op, const U8 idx) {
  return (op->outString + (outStringChars * idx));
}

//===========================
//=== subclass definitions

//----- switch
static const char* op_sw_instring = "VALUE   TOGGLE  ";
static const char* op_sw_outstring = "VALUE  ";
static const char* op_sw_opstring = "SWITCH";

static void op_sw_in_val(op_sw_t* sw, const S32* v) {
  printf("switch at %d received value %d\n", (int)sw, *v);
  if(sw->tog) {
    if (v == 0) {
      return;
    } else {
      sw->val = (sw->val == 0); 
    }
  } else {
    sw->val = (v != 0);
  }
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
static const char* op_enc_instring = "PIN1    PIN2    ";
static const char* op_enc_outstring = "DIRECTION";
static const char* op_enc_opstring = "ENCODER";
static const int enc_map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };

static void op_enc_in_pin1(op_enc_t* enc, const S32* v) {
  enc->pos_now = ((enc->pos_now) & 1) | ((U32)*v & 1) ;
  if (enc->pos_now != enc->pos_old) {
    enc->val = enc_map[enc->pos_old][enc->pos_now];
    enc->pos_old = enc->pos_now;
    ctl_go(enc->outs[0], &(enc->val));
  }
}

static void op_enc_in_pin2(op_enc_t* enc, const S32* v) {
  enc->pos_now = ((enc->pos_now) & 2) | (((U32)*v & 1) << 1) ;
  if (enc->pos_now != enc->pos_old) {
    enc->val = enc_map[enc->pos_old][enc->pos_now];
    enc->pos_old = enc->pos_now;
    ctl_go(enc->outs[0], &(enc->val));
  }
}

static ctl_in_t op_enc_inputs[2] = {
  (ctl_in_t)&op_enc_in_pin1,
  (ctl_in_t)&op_enc_in_pin2
};

void op_enc_init(op_enc_t* enc) {
  enc->super.numInputs = 2;
  enc->super.numOutputs = 1;
  enc->outs[0] = -1;
  enc->super.in = op_enc_inputs;
  enc->super.out = enc->outs;
  enc->super.opString = op_enc_opstring;
  enc->super.size = sizeof(op_enc_t);
}

//---- adder
static const char* op_add_instring = "A       B       B_TRIG  ";
static const char* op_add_outstring = "SUM";
static const char* op_add_opstring = "ADDER";

static void op_add_in_a(op_add_t* add, const S32* v) {
  add->a = *v;
  add->val = add->a + add->b;
  ctl_go(add->outs[0], &(add->val));
}

static void op_add_in_b(op_add_t* add, const S32* v) {
  add->b = *v;
  add->val = add->a + add->b;
  if(add->btrig) {
    ctl_go(add->outs[0], &(add->val));
  }
}

static void op_add_in_btrig(op_add_t* add, const S32* v) {
  add->btrig = (v != 0);
}


static ctl_in_t op_add_inputs[3] = {
  (ctl_in_t)&op_add_in_a,
  (ctl_in_t)&op_add_in_b, 
  (ctl_in_t)&op_add_in_btrig
};

void op_add_init(op_add_t* add) {
  add->super.numInputs = 3;
  add->super.numOutputs = 1;
  add->outs[0] = -1;
  add->super.in = op_add_inputs;
  add->super.out = add->outs;
  add->super.opString = op_add_opstring;
  add->super.size = sizeof(op_add_t);
}

//----- multiplier
static const char* op_mul_instring = "A       B       B_TRIG  ";
static const char* op_mul_outstring = "SUM";
static const char* op_mul_opstring = "ADDER";

static void op_mul_in_a(op_mul_t* mul, const S32* v) {
  mul->a = v;
  mul->val = mul->a * mul->b;
  ctl_go(mul->outs[0], &(mul->val));
}

static void op_mul_in_b(op_mul_t* mul, const S32* v) {
  mul->b = v;
  mul->val = mul->a * mul->b;
  if(mul->btrig) {
    ctl_go(mul->outs[0], &(mul->val));
  }
}

static void op_mul_in_btrig(op_mul_t* mul, const S32* v) {
  mul->btrig = (v != 0);
}


static ctl_in_t op_mul_inputs[3] = {
  (ctl_in_t)&op_mul_in_a,
  (ctl_in_t)&op_mul_in_b, 
  (ctl_in_t)&op_mul_in_btrig
};


void op_mul_init(op_mul_t* mul) {
}

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
