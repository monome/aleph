// asf
#include "print_funcs.h"

// avr32_lib
#include "global.h"

// bees
#include "net_protected.h"
#include "pickle.h"
#include "op_param.h"

//-------------------------------------------------
//----- descriptor
static const char* op_param_instring = "IDX\0  ";
static const char* op_param_outstring = "VAL\0   ";
static const char* op_param_opstring = "PARAM";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_param_in_event(op_param_t* param, const io_t v);
// pickle / unpickle
static u8* op_param_pickle(op_param_t* param, u8* dst);
static u8* op_param_unpickle(op_param_t* param, const u8* src);

// calculate output value
//static void op_param_calc(op_param_t* param); 

// array of input functions 
static op_in_fn op_param_in[1] = {
  (op_in_fn)&op_param_in_event,
};

//----- external function definition

/// initialize
void op_param_init(void* op) {
  op_param_t* param = (op_param_t*)op;

  // superclass functions
  param->super.in_fn = op_param_in;
  param->super.pickle = (op_pickle_fn) (&op_param_pickle);
  param->super.unpickle = (op_unpickle_fn) (&op_param_unpickle);
  
  // superclass state
  param->super.numInputs = 1;
  param->super.numOutputs = 1;
  param->outs[0] = -1;
 
  param->super.in_val = param->in_val;
  param->in_val[0] = &(param->event);
  param->super.out = param->outs;
  param->super.opString = op_param_opstring;
  param->super.inString = op_param_instring;
  param->super.outString = op_param_outstring;
  param->super.type = eOpParam;

  // class state
  param->event = 0;
  param->ticks = tcTicks;
  param->interval = 1000;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input event
static void op_param_in_event(op_param_t* param, const io_t v) {
  param->event = v;
  net_activate(param->outs[0], bfin_get_param(v), param);   
}


// serialization
u8* op_param_pickle(op_param_t* param, u8* dst) {
  // no state
  return dst;
}

u8* op_param_unpickle(op_param_t* param, const u8* src) {
  // no state
  return (u8*)src;
}
