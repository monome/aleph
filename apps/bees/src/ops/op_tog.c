#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_tog.h"

//-------------------------------------------------
//----- descriptor
static const char* op_tog_instring = "STATE\0  MUL\0    ";
static const char* op_tog_outstring = "VAL\0    ";
static const char* op_tog_opstring = "TOG";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_tog_in_state(op_tog_t* tog, const io_t v);
static void op_tog_in_mul(op_tog_t* tog, const io_t );
// pickle / unpickle
static u8* op_tog_pickle(op_tog_t* tog, u8* dst);
static u8* op_tog_unpickle(op_tog_t* tog, const u8* src);

// array of input functions 
static op_in_fn op_tog_in[2] = {
  (op_in_fn)&op_tog_in_state,
  (op_in_fn)&op_tog_in_mul
};

//----- external function definition

/// initialize
void op_tog_init(void* op) {
  op_tog_t* tog = (op_tog_t*)op;

  // superclass functions
  tog->super.in_fn = op_tog_in;
  tog->super.pickle = (op_pickle_fn) (&op_tog_pickle);
  tog->super.unpickle = (op_unpickle_fn) (&op_tog_unpickle);
  
  // superclass state
  tog->super.numInputs = 2;
  tog->super.numOutputs = 1;
  tog->outs[0] = -1;
 
  tog->super.in_val = tog->in_val;
  tog->in_val[0] = &(tog->state);
  tog->in_val[1] = &(tog->mul);

  tog->super.out = tog->outs;
  tog->super.opString = op_tog_opstring;
  tog->super.inString = op_tog_instring;
  tog->super.outString = op_tog_outstring;
  tog->super.type = eOpTog;

  // class state
  tog->state = 0;
  tog->mul = OP_ONE;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_tog_in_state(op_tog_t* tog, const io_t v) {
  if ( (v) > 0) {
    if ((tog->state) == 0) {
      tog->state = tog->mul;
    } else {
      tog->state = 0;
    }
    net_activate(tog->outs[0], tog->state, tog);
  }
}

// input multiplier
static void op_tog_in_mul(op_tog_t* tog, const io_t v) {
  tog->mul = v;
  if (tog->state > 0) {
    tog->state = (v);
    net_activate(tog->outs[0], tog->state, tog);
  }
}


// serialization
u8* op_tog_pickle(op_tog_t* tog, u8* dst) {
  // store state variables
  dst = pickle_io(tog->state, dst);
  dst = pickle_io(tog->mul, dst);
  return dst;
}

u8* op_tog_unpickle(op_tog_t* tog, const u8* src) {
  // retreive state variables
  src = unpickle_io(src, (u32*)&(tog->state));
  src = unpickle_io(src, (u32*)&(tog->mul));
   return (u8*)src;
}
