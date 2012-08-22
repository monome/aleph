/* ctl_op.c
 * aleph-avr32
 *
 * operator implmentation (base and derived classes)
 */

#include <stdio.h>

#include "types.h"
#include "net.h"
//#include "ui.h"
#include "param.h"
#include "op_math.h"
#include "op.h"


// operator class registry
// must be laid out identically to eOpId enum
const op_desc_t op_registry[numOpClasses] = {
{ "SW", sizeof(op_sw_t) },
{ "ENC", sizeof(op_enc_t) },
{ "ADD", sizeof(op_add_t) },
{ "MUL", sizeof(op_mul_t) },
{ "GATE", sizeof(op_gate_t) },
// { "ACCUMULATE", sizeof(op_accum_t) },
// { "SELECT", sizeof(op_sel_t) },
// { "LINEAR MAP", sizeof(op_lin_t) },
//{ "PARAMETER", sizeof(op_param_t) },
// { "PRESET", sizeof(op_preset_t) }
};


// input and output strings are all the same length, concatenated
// this is lazy, but also efficient.
static const u8 inStringChars = 8;
static const u8 outStringChars = 8;

//===============================================
//===  base class definittions
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
}

//============================================
//=== subclass definitions

//-------------------------------------------------
//----- switch
static const char* op_sw_instring = "VAL     TOG     MUL     ";
static const char* op_sw_outstring = "VAL     ";
static const char* op_sw_opstring = "SW";

static void op_sw_in_state(op_sw_t* sw, const io_t* v) {
  if (sw->tog) {
    // toggle mode, sw state toggles on positive input
    if ( *v > 0) {
      if (sw->state > 0) { 
	sw->state = 0; 
      } else {
	sw->state = sw->mul;
      }
      net_activate(sw->outs[0], sw->state);
    } 
  } else {
    // momentary mode, sw value takes input
    sw->state = (((*v) > 0) ? sw->mul : 0);
    net_activate(sw->outs[0], sw->state);
  }
}

static void op_sw_in_tog(op_sw_t* sw, const io_t* v) {
  sw->tog = (io_t)(*v > 0);
  /*
  if (sw-> val > 0) { sw->state = 0; }
  else { sw->state = sw->mul; }
  net_activate(sw->outs[0], sw->state);
  */
 }

static void op_sw_in_mul(op_sw_t* sw, const io_t* v) {
  sw->mul = *v;
  sw->state = (((*v) > 0) ? sw->mul : 0);
  net_activate(sw->outs[0], sw->state);
 }

static op_in_func_t op_sw_in_func[3] = {
  (op_in_func_t)&op_sw_in_state,
  (op_in_func_t)&op_sw_in_tog,
  (op_in_func_t)&op_sw_in_mul
};

void op_sw_init(op_sw_t* sw) {
  sw->super.numInputs = 3;
  sw->super.numOutputs = 1;
  sw->outs[0] = -1;
  sw->super.in_func = op_sw_in_func;
  sw->super.in_val = sw->in_val;
  sw->in_val[0] = &(sw->state);
  sw->in_val[1] = &(sw->mul);
  sw->in_val[2] = &(sw->tog);
  sw->super.out = sw->outs;
  sw->super.opString = op_sw_opstring;
  sw->super.inString = op_sw_instring;
  sw->super.outString = op_sw_outstring;
  sw->super.type = eOpSwitch;
  sw->super.status = eSysCtlOp;
}

//-------------------------------------------------
//----- encoder

static const char* op_enc_instring = "DIR     MIN     MAX     STEP    WRAP    ";
static const char* op_enc_outstring = "VAL     WRAP    ";
static const char* op_enc_opstring = "ENC";
static void op_enc_perform(op_enc_t* enc);

// step
static void op_enc_in_step(op_enc_t* enc, const io_t* v) {
  enc->step = *v;
  // op_enc_perform(enc);
}

// move
static void op_enc_in_move(op_enc_t* enc, const io_t* v) {
  //  enc->val += enc->step * (*v); 
  enc->val = OP_ADD(enc->val, OP_MUL(enc->step, *v));
  op_enc_perform(enc);
}
// max
static void op_enc_in_min(op_enc_t* enc, const io_t* v) {
  enc->min = *v;
  op_enc_perform(enc);
}

// max
static void op_enc_in_max(op_enc_t* enc, const io_t* v) {
  enc->max = *v;
  op_enc_perform(enc);
}

// wrap behavior
static void op_enc_in_wrap(op_enc_t* enc, const io_t* v) {
  enc->wrap = (*v > 0);
  //  op_enc_perform(enc);
}

// perform wrapping and output
static void op_enc_perform(op_enc_t* enc) {
  io_t wrap = 0;
  io_t dif = 0;
  if (wrap) { // wrapping...
    // if value needs wrapping, output the applied difference
    while (enc->val > enc->max) { 
      dif = OP_SUB(enc->min, enc->max);
      wrap = OP_ADD(wrap, dif);
      enc->val = OP_ADD(enc->val, dif);
    }
    while (enc->val < enc->min) { 
      dif = OP_SUB(enc->max, enc->min);
      wrap = OP_ADD(wrap, dif);
      enc->val = OP_ADD(enc->val, dif);
    }
  } else { // saturating...
    if (enc->val > enc->max) {
      enc->val = enc->max;
      dif = 1; // force wrap output with value 0
    }
    if (enc->val < enc->min) {
      enc->val = enc->min;
      dif = -1; // force wrap output with value 0
    }
  }
  // output the value
  net_activate(enc->outs[0], enc->val);

  // output the wrap amount
  if (dif != 0) {
    net_activate(enc->outs[1], wrap);  
  }
}

// input functionsm
static op_in_func_t op_enc_in_func[5] = {
  (op_in_func_t)&op_enc_in_move,
  (op_in_func_t)&op_enc_in_min,
  (op_in_func_t)&op_enc_in_max,
  (op_in_func_t)&op_enc_in_step,
  (op_in_func_t)&op_enc_in_wrap,
};

// default value

void op_enc_init(op_enc_t* enc) {
  enc->super.numInputs = 5;
  enc->super.numOutputs = 2;
  enc->outs[0] = -1;
  enc->outs[1] = -1;
  enc->super.in_func = op_enc_in_func;
  enc->super.in_val = enc->in_val;
  enc->super.out = enc->outs;
  enc->super.opString = op_enc_opstring;
  enc->super.inString = op_enc_instring;
  enc->super.outString = op_enc_outstring;
  enc->super.type = eOpEnc;
  enc->super.status = eSysCtlOp;

  enc->in_val[0] = &(enc->move);
  enc->in_val[1] = &(enc->min);
  enc->in_val[2] = &(enc->max);
  enc->in_val[3] = &(enc->step);
  enc->in_val[4] = &(enc->wrap);
}

//-------------------------------------------------
//---- adder
static const char* op_add_instring = "A       B       B_TRIG  ";
static const char* op_add_outstring = "SUM     ";
static const char* op_add_opstring = "ADDER";

static void op_add_in_a(op_add_t* add, const io_t* v) {
  add->a = *v;
  add->val = OP_ADD(add->a, add->b);
  net_activate(add->outs[0], add->val);
}

static void op_add_in_b(op_add_t* add, const io_t* v) {
  add->b = *v;
  add->val = OP_ADD(add->a, add->b);
  if(add->btrig) {
    net_activate(add->outs[0], add->val);
  }
}

static void op_add_in_btrig(op_add_t* add, const io_t* v) {
  add->btrig = (v != 0);
}


static op_in_func_t op_add_in_func[3] = {
  (op_in_func_t)&op_add_in_a,
  (op_in_func_t)&op_add_in_b, 
  (op_in_func_t)&op_add_in_btrig
};

void op_add_init(op_add_t* add) {
  add->super.numInputs = 3;
  add->super.numOutputs = 1;
  add->outs[0] = -1;
  add->super.in_func = op_add_in_func;
  add->super.in_val = add->in_val;
  add->super.out = add->outs;
  add->super.opString = op_add_opstring;
  add->super.inString = op_add_instring;
  add->super.outString = op_add_outstring;
  add->super.type = eOpAdd;  
  add->super.status = eUserOp;  

  add->in_val[0] = &(add->a);
  add->in_val[1] = &(add->b);
  add->in_val[2] = &(add->btrig);
}

//-------------------------------------------------
//----- multiplier
static const char* op_mul_instring = "A       B       B_TRIG  ";
static const char* op_mul_outstring = "PRODUCT ";
static const char* op_mul_opstring = "MULTIPLIER";

static void op_mul_in_a(op_mul_t* mul, const io_t* v) {
  // printf("mul at %d received A %d\n", (int)mul, (int)*v);
  mul->a = *v;
  mul->val = OP_MUL(mul->a, mul->b);
  net_activate(mul->outs[0], mul->val);
}

static void op_mul_in_b(op_mul_t* mul, const io_t* v) {
  //printf("mul at %d received B %d\n", (int)mul, (int)*v);
  mul->b = *v;
  mul->val = OP_MUL(mul->a, mul->b);
  if(mul->btrig) {
    net_activate(mul->outs[0], mul->val);
  }
}

static void op_mul_in_btrig(op_mul_t* mul, const io_t* v) {
  //printf("mul at %d received BTRIG %d\n", (int)mul, (int)*v);
  mul->btrig = (io_t)(v != 0);
}

static op_in_func_t op_mul_in_func[3] = {
  (op_in_func_t)&op_mul_in_a,
  (op_in_func_t)&op_mul_in_b, 
  (op_in_func_t)&op_mul_in_btrig
};

void op_mul_init(op_mul_t* mul) {
  mul->super.numInputs = 3;
  mul->super.numOutputs = 1;
  mul->outs[0] = -1;
  mul->super.in_func = op_mul_in_func;
  mul->super.in_val = mul->in_val;
  mul->super.out = mul->outs;
  mul->super.opString = op_mul_opstring;
  mul->super.inString = op_mul_instring;
  mul->super.outString = op_mul_outstring;
  mul->super.type = eOpMul;  
  mul->super.status = eUserOp;  
 
  mul->in_val[0] = &(mul->a);
  mul->in_val[1] = &(mul->b);
  mul->in_val[2] = &(mul->btrig);
}

//-------------------------------------------------
//----- gate
static const char* op_gate_instring = "VALUE   GATE    STORE   ";
static const char* op_gate_outstring = "GATED   ";
static const char* op_gate_opstring = "GATE";

static void op_gate_in_value(op_gate_t* gate, const io_t* v) {
  gate->val = *v;
  if(gate->gate != 0) {
    net_activate(gate->outs[0], gate->val);
  }
}

static void op_gate_in_gate(op_gate_t* gate, const io_t* v) {
  gate->gate = (io_t)(*v != 0);
  if (gate->store) {
    net_activate(gate->outs[0], gate->val);
  }
}

static void op_gate_in_store(op_gate_t* gate, const io_t* v) {
  gate->store = (*v != 0);
}

static op_in_func_t op_gate_in_func[3] = {
  (op_in_func_t)&op_gate_in_value,
  (op_in_func_t)&op_gate_in_gate, 
  (op_in_func_t)&op_gate_in_store
};

void op_gate_init(op_gate_t* gate) {
  gate->super.numInputs = 3;
  gate->super.numOutputs = 1;
  gate->outs[0] = -1;
  gate->super.in_func = op_gate_in_func;
  gate->super.in_val = gate->in_val;
  gate->super.out = gate->outs;
  gate->super.opString = op_gate_opstring;
  gate->super.inString = op_gate_instring;
  gate->super.outString = op_gate_outstring;
  gate->super.type = eOpGate;  
  gate->super.status = eUserOp;

  gate->in_val[0] = &(gate->val);
  gate->in_val[1] = &(gate->gate);
  gate->in_val[2] = &(gate->store);
}

//-------------------------------------------------
//------ accumulator
#if 0 // needs work
static const char* op_accum_instring = "VALUE   COUNT   MIN     MAX     WRAP    ";
static const char* op_accum_outstring = "VALUE   WRAP    ";
static const char* op_accum_opstring = "ACCUMULATOR";

static void op_accum_boundscheck(op_accum_t* accum) {
  if(accum->val > accum->max) {
    if(accum->carry != 0) {
      while(accum->val > accum->max) {
        //accum->val -= (accum->max > 0 ? accum->max : accum->max * -1);
	if(accum->max > 0) {
	  accum->val = OP_SUB(accum->val, accum_max);
	} else {
	  accum->val = OP_ADD(accum->val, accum_max);
	}
      }
      net_activate(accum->outs[1], accum->val); // carry output with wrapped value
    } else {
      accum->val = accum->max;
    }
  }
  if(accum->val < accum->min) {
    if(accum->carry) {
      while(accum->val < accum->min) {
	if(accum->min < 0) {
	  accum->val = OP_SUB(accum->val, accum_min);
	} else {
	  accum->val = OP_ADD(accum->val, accum_min);
	}
      }
      net_activate(accum->outs[1], accum->val); // carry output with wrapped value
    } else {
      accum->val = accum->min;
    }
  }  
}

static void op_accum_in_value(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received VALUE %d\n", (int)accum, (int)*v);
  accum->val = *v;
  op_accum_boundscheck(accum);
  net_activate(accum->outs[0], accum->val);
}

static void op_accum_in_count(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received COUNT %d\n", (int)accum, (int)*v);
  acum->val = OP_ADD(accum->val, *v);
  op_accum_boundscheck(accum);
  net_activate(accum->outs[0], accum->val);
}

static void op_accum_in_min(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received MIN %d\n", (int)accum, (int)*v);
  accum->min = *v;
}

static void op_accum_in_max(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received MAX %d\n", (int)accum, (int)*v);
  accum->max = *v;
}

static void op_accum_in_carry(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received CARRY %d\n", (int)accum, (int)*v);
  accum->carry = (io_t)(v != 0);
}

static op_in_func_t op_accum_inputs[5] = {
(op_in_func_t)&op_accum_in_value,
(op_in_func_t)&op_accum_in_count,
(op_in_func_t)&op_accum_in_min, 
(op_in_func_t)&op_accum_in_max,
(op_in_func_t)&op_accum_in_carry
};

void op_accum_init(op_accum_t* accum) {
  accum->super.numInputs = 5;
  accum->super.numOutputs = 2;
  accum->outs[0] = -1;
  accum->outs[1] = -1;
  accum->super.in_func = op_accum_inputs;
  accum->super.out = accum->outs;
  accum->super.opString = op_accum_opstring;
  accum->super.inString = op_accum_instring;
  accum->super.outString = op_accum_outstring;
  accum->super.type = eOpAccum; 
  accum->super.status = eUserOp;  
//  accum->super.size = sizeof(op_accum_t);
}


//-------------------------------------------------
//------ range selector
void op_sel_init(op_sel_t* sel);

//-------------------------------------------------
//----- linear map
void op_lin_init(op_lin_t* lin);

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
void op_preset_init(op_preset_t* preset);

#endif 
