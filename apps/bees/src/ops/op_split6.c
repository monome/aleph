#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_split6.h"

//-------------------------------------------------
//----- descriptor
static const char* op_split6_instring = "X\0      ";
static const char* op_split6_outstring = "A\0      B\0      C\0      D\0      E\0      F\0	 ";
static const char* op_split6_opstring = "Y6";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_split6_in_val(op_split6_t* split6, const io_t v);
// pickle / unpickle
static u8* op_split6_pickle(op_split6_t* split6, u8* dst);
static u8* op_split6_unpickle(op_split6_t* split6, const u8* src);

// array of input functions 
static op_in_fn op_split6_in[1] = {
  (op_in_fn)&op_split6_in_val,
};

//----- external function definition

/// initialize
void op_split6_init(void* op) {
  op_split6_t* split6 = (op_split6_t*)op;

  // superclass functions
  split6->super.in_fn = op_split6_in;
  split6->super.pickle = (op_pickle_fn) (&op_split6_pickle);
  split6->super.unpickle = (op_unpickle_fn) (&op_split6_unpickle);
  
  // superclass state
  split6->super.numInputs = 1;
  split6->super.numOutputs = 6;
  split6->outs[0] = -1;
  split6->outs[1] = -1;
  split6->outs[2] = -1;
  split6->outs[3] = -1;
  split6->outs[4] = -1;
  split6->outs[5] = -1;
 
  split6->super.in_val = split6->in_val;
  split6->in_val[0] = &(split6->val);

  split6->super.out = split6->outs;
  split6->super.opString = op_split6_opstring;
  split6->super.inString = op_split6_instring;
  split6->super.outString = op_split6_outstring;
  split6->super.type = eOpSplit6;

  // class state
  split6->val = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_split6_in_val(op_split6_t* split6, const io_t v) {
  split6->val = v;
  net_activate(split6->outs[0], split6->val, split6);
  net_activate(split6->outs[1], split6->val, split6);
  net_activate(split6->outs[2], split6->val, split6);
  net_activate(split6->outs[3], split6->val, split6);
  net_activate(split6->outs[4], split6->val, split6);
  net_activate(split6->outs[5], split6->val, split6);
}

// serialization
u8* op_split6_pickle(op_split6_t* split6, u8* dst) {
  // no state
  return dst;
}

u8* op_split6_unpickle(op_split6_t* split6, const u8* src) {
  // no state
  return (u8*)src;
}
