#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_split4.h"

//-------------------------------------------------
//----- descriptor
static const char* op_split4_instring = "X\0      ";
static const char* op_split4_outstring = "A\0      B\0      C\0      D\0      ";
static const char* op_split4_opstring = "Y4";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_split4_in_val(op_split4_t* split4, const io_t v);
// pickle / unpickle
static u8* op_split4_pickle(op_split4_t* split4, u8* dst);
static u8* op_split4_unpickle(op_split4_t* split4, const u8* src);

// array of input functions 
static op_in_fn op_split4_in[1] = {
  (op_in_fn)&op_split4_in_val,
};

//----- external function definition

/// initialize
void op_split4_init(void* op) {
  op_split4_t* split4 = (op_split4_t*)op;

  // superclass functions
  split4->super.in_fn = op_split4_in;
  split4->super.pickle = (op_pickle_fn) (&op_split4_pickle);
  split4->super.unpickle = (op_unpickle_fn) (&op_split4_unpickle);
  
  // superclass state
  split4->super.numInputs = 1;
  split4->super.numOutputs = 4;
  split4->outs[0] = -1;
  split4->outs[1] = -1;
  split4->outs[2] = -1;
  split4->outs[3] = -1;
 
  split4->super.in_val = split4->in_val;
  split4->in_val[0] = &(split4->val);

  split4->super.out = split4->outs;
  split4->super.opString = op_split4_opstring;
  split4->super.inString = op_split4_instring;
  split4->super.outString = op_split4_outstring;
  split4->super.type = eOpSplit4;

  // class state
  split4->val = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_split4_in_val(op_split4_t* split4, const io_t v) {
  split4->val = v;
  net_activate(split4->outs[0], split4->val, split4);
  net_activate(split4->outs[1], split4->val, split4);
  net_activate(split4->outs[2], split4->val, split4);
  net_activate(split4->outs[3], split4->val, split4);
}

// serialization
u8* op_split4_pickle(op_split4_t* split4, u8* dst) {
  // no state
  return dst;
}

u8* op_split4_unpickle(op_split4_t* split4, const u8* src) {
  // no state
  return (u8*)src;
}
