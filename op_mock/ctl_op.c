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

//===============================================
//===  base class definitions
const char* ctl_op_in_name(ctl_op_t* op, const U8 idx) {
  static char str[64];
  U8 i;
  // str = (op->inString + (inStringChars * idx));
  for(i=0; i<inStringChars; i++) {
    str[i] = *(op->inString + (inStringChars * idx) + i);
  }
  str[inStringChars] = '\0';
  return str;
}

const char* ctl_op_out_name(ctl_op_t* op, const U8 idx) {
  static char str[64];
  U8 i;
//  str = (op->outString + (outStringChars * idx));
  
  for(i=0; i<outStringChars; i++) {
    str[i] = *(op->outString + (outStringChars * idx) + i);
  }
  str[outStringChars] = '\0';
  return str;
}

//============================================
//=== subclass definitions

//-------------------------------------------------
//----- switch
static const char* op_sw_instring = "VALUE   TOGGLE  ";
static const char* op_sw_outstring = "VALUE  ";
static const char* op_sw_opstring = "SWITCH";

static void op_sw_in_val(op_sw_t* sw, const S32* v) {
  printf("switch at %d received VALUE %d\n", (int)sw, *v);
  if(sw->tog) {
    if (v == 0) {
      return;
    } else {
      sw->val = (sw->val == 0); 
    }
  } else {
    sw->val = (v != 0);
  }
  //if (sw->outs[0] >= 0) {
    ctl_go(sw->outs[0], &(sw->val));
  //}
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
  sw->super.inString = op_sw_instring;
  sw->super.outString = op_sw_outstring;
  sw->super.size = sizeof(op_sw_t);
}

//-------------------------------------------------
//----- encoder
static const char* op_enc_instring = "PIN1    PIN2    ";
static const char* op_enc_outstring = "DIRECTION";
static const char* op_enc_opstring = "ENCODER";
static const int enc_map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };

static void op_enc_in_pin1(op_enc_t* enc, const S32* v) {
  printf("enc at %d received PIN1 %d\n", (int)enc, *v);
  enc->pos_now = ((enc->pos_now) & 1) | ((U32)*v & 1) ;
  if (enc->pos_now != enc->pos_old) {
    enc->val = enc_map[enc->pos_old][enc->pos_now];
    enc->pos_old = enc->pos_now;
    ctl_go(enc->outs[0], &(enc->val));
  }
}

static void op_enc_in_pin2(op_enc_t* enc, const S32* v) {
  printf("enc at %d received PIN2 %d\n", (int)enc, *v);
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
  enc->super.inString = op_enc_instring;
  enc->super.outString = op_enc_outstring;
  enc->super.size = sizeof(op_enc_t);
}

//-------------------------------------------------
//---- adder
static const char* op_add_instring = "A       B       B_TRIG  ";
static const char* op_add_outstring = "SUM     ";
static const char* op_add_opstring = "ADDER";

static void op_add_in_a(op_add_t* add, const S32* v) {
  printf("add at %d received A %d\n", (int)add, *v);
  add->a = *v;
  add->val = add->a + add->b;
  ctl_go(add->outs[0], &(add->val));
}

static void op_add_in_b(op_add_t* add, const S32* v) {
  printf("add at %d received B %d\n", (int)add, *v);
  add->b = *v;
  add->val = add->a + add->b;
  if(add->btrig) {
    ctl_go(add->outs[0], &(add->val));
  }
}

static void op_add_in_btrig(op_add_t* add, const S32* v) {
  printf("add at %d received BTRIG %d\n", (int)add, *v);
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
  add->super.inString = op_add_instring;
  add->super.outString = op_add_outstring;
  add->super.size = sizeof(op_add_t);
}

//-------------------------------------------------
//----- multiplier
static const char* op_mul_instring = "A       B       B_TRIG  ";
static const char* op_mul_outstring = "PRODUCT ";
static const char* op_mul_opstring = "MULTIPLIER";

static void op_mul_in_a(op_mul_t* mul, const S32* v) {
  printf("mul at %d received A %d\n", (int)mul, *v);
  mul->a = *v;
  mul->val = mul->a * mul->b;
  ctl_go(mul->outs[0], &(mul->val));
}

static void op_mul_in_b(op_mul_t* mul, const S32* v) {
  printf("mul at %d received B %d\n", (int)mul, *v);
  mul->b = *v;
  mul->val = mul->a * mul->b;
  if(mul->btrig) {
    ctl_go(mul->outs[0], &(mul->val));
  }
}

static void op_mul_in_btrig(op_mul_t* mul, const S32* v) {
  printf("mul at %d received BTRIG %d\n", (int)mul, *v);
  mul->btrig = (v != 0);
}

static ctl_in_t op_mul_inputs[3] = {
  (ctl_in_t)&op_mul_in_a,
  (ctl_in_t)&op_mul_in_b, 
  (ctl_in_t)&op_mul_in_btrig
};

void op_mul_init(op_mul_t* mul) {
  mul->super.numInputs = 3;
  mul->super.numOutputs = 1;
  mul->outs[0] = -1;
  mul->super.in = op_mul_inputs;
  mul->super.out = mul->outs;
  mul->super.opString = op_mul_opstring;
  mul->super.inString = op_mul_instring;
  mul->super.outString = op_mul_outstring;
  mul->super.size = sizeof(op_mul_t);
}

//-------------------------------------------------
//----- gate
static const char* op_gate_instring = "VALUE   GATE    STORE   ";
static const char* op_gate_outstring = "GATED   ";
static const char* op_gate_opstring = "GATE";

static void op_gate_in_value(op_gate_t* gate, const S32* v) {
  printf("gate at %d received VALUE %d\n", (int)gate, *v);
  gate->val = *v;
  if(gate->gate) {
    ctl_go(gate->outs[0], &(gate->val));
  }
}

static void op_gate_in_gate(op_gate_t* gate, const S32* v) {
  printf("gate at %d received GATE %d\n", (int)gate, *v);
  gate->val = (*v != 0);
  if (gate->val) {
    if (gate->store) {
      ctl_go(gate->outs[0], &(gate->val));
    }
  }
}

static void op_gate_in_store(op_gate_t* gate, const S32* v) {
  printf("gate at %d received STORE %d\n", (int)gate, *v);
  gate->store = (*v != 0);
}

static ctl_in_t op_gate_inputs[3] = {
  (ctl_in_t)&op_gate_in_value,
  (ctl_in_t)&op_gate_in_gate, 
  (ctl_in_t)&op_gate_in_store
};

void op_gate_init(op_gate_t* gate) {
  gate->super.numInputs = 3;
  gate->super.numOutputs = 1;
  gate->outs[0] = -1;
  gate->super.in = op_gate_inputs;
  gate->super.out = gate->outs;
  gate->super.opString = op_gate_opstring;
  gate->super.inString = op_gate_instring;
  gate->super.outString = op_gate_outstring;
  gate->super.size = sizeof(op_gate_t);
}

//-------------------------------------------------
//------ accumulator
static const char* op_accum_instring = "VALUE   COUNT   MIN     MAX     CARRY   ";
static const char* op_accum_outstring = "VALUE   CARRY";
static const char* op_accum_opstring = "ACCUMULATOR";

static void op_accum_boundscheck(op_accum_t* accum) {
  if(accum->val > accum->max) {
    if(accum->carry) {
      while(accum->val > accum->max) {
        accum->val -= (accum->max > 0 ? accum->max : accum->max * -1);
      }
      ctl_go(accum->outs[1], &(accum->val)); // carry output with wrapped value
    } else {
      accum->val = accum->max;
    }
  }
  if(accum->val < accum->min) {
    if(accum->carry) {
      while(accum->val < accum->min) {
        accum->val += (accum->min > 0 ? accum->min : accum->min * -1);
      }
      ctl_go(accum->outs[1], &(accum->val)); // carry output with wrapped value
    } else {
      accum->val = accum->min;
    }
  }  
}

static void op_accum_in_value(op_accum_t* accum, const S32* v) {
  printf("accum at %d received VALUE %d\n", (int)accum, *v);
  accum->val = *v;
  op_accum_boundscheck(accum);
  ctl_go(accum->outs[0], &(accum->val));
}

static void op_accum_in_count(op_accum_t* accum, const S32* v) {
  printf("accum at %d received COUNT %d\n", (int)accum, *v);
  accum->val += *v;
  op_accum_boundscheck(accum);
  ctl_go(accum->outs[0], &(accum->val));
}

static void op_accum_in_min(op_accum_t* accum, const S32* v) {
  printf("accum at %d received MIN %d\n", (int)accum, *v);
  accum->min = *v;
}

static void op_accum_in_max(op_accum_t* accum, const S32* v) {
  printf("accum at %d received MAX %d\n", (int)accum, *v);
  accum->max = *v;
}

static void op_accum_in_carry(op_accum_t* accum, const S32* v) {
  printf("accum at %d received CARRY %d\n", (int)accum, *v);
  accum->carry = (v != 0);
}

static ctl_in_t op_accum_inputs[5] = {
(ctl_in_t)&op_accum_in_value,
(ctl_in_t)&op_accum_in_count,
(ctl_in_t)&op_accum_in_min, 
(ctl_in_t)&op_accum_in_max,
(ctl_in_t)&op_accum_in_carry
};

void op_accum_init(op_accum_t* accum) {
  accum->super.numInputs = 5;
  accum->super.numOutputs = 2;
  accum->outs[0] = -1;
  accum->outs[1] = -1;
  accum->super.in = op_accum_inputs;
  accum->super.out = accum->outs;
  accum->super.opString = op_accum_opstring;
  accum->super.inString = op_accum_instring;
  accum->super.outString = op_accum_outstring;
  accum->super.size = sizeof(op_accum_t);
}

//-------------------------------------------------
//------ range selector
void op_sel_init(op_sel_t* sel);

//-------------------------------------------------
//----- linear map
void op_lin_init(op_lin_t* lin);

//-------------------------------------------------
//---- param value receiver
void op_param_init(op_param_t* param);

//-------------------------------------------------
//----- preset manipulator
void op_preset_init(op_preset_t* preset);
