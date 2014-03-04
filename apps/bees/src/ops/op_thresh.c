#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_thresh.h"

//-------------------------------------------------
//----- descriptor
static const char* op_thresh_instring =  "IN\0     LIM\0    ";
static const char* op_thresh_outstring = "LO\0     HI\0     ";
static const char* op_thresh_opstring =  "THRESH";

//-------------------------------------------------
//----- static function declaration

// set inputs
static void op_thresh_in_state(op_thresh_t* thresh, const io_t v);
static void op_thresh_in_lim(op_thresh_t* thresh, const io_t );
// pickle / unpickle
static u8* op_thresh_pickle(op_thresh_t* thresh, u8* dst);
static u8* op_thresh_unpickle(op_thresh_t* thresh, const u8* src);

// array of input functions 
static op_in_fn op_thresh_in[2] = {
  (op_in_fn)&op_thresh_in_state,
  (op_in_fn)&op_thresh_in_lim
};

//----- external function definition

/// initialize
void op_thresh_init(void* op) {
  op_thresh_t* thresh = (op_thresh_t*)op;

  // superclass functions
  thresh->super.in_fn = op_thresh_in;
  thresh->super.pickle = (op_pickle_fn) (&op_thresh_pickle);
  thresh->super.unpickle = (op_unpickle_fn) (&op_thresh_unpickle);
  
  // superclass state
  thresh->super.numInputs = 2;
  thresh->super.numOutputs = 2;
  thresh->outs[0] = -1;
  thresh->outs[1] = -1;
 
  thresh->super.in_val = thresh->in_val;
  //  thresh->in_val[0] = &(thresh->state);
  thresh->in_val[0] = &(thresh->state);
  thresh->in_val[1] = &(thresh->lim);

  thresh->super.out = thresh->outs;
  thresh->super.opString = op_thresh_opstring;
  thresh->super.inString = op_thresh_instring;
  thresh->super.outString = op_thresh_outstring;
  thresh->super.type = eOpThresh;

  // class state
  thresh->state = 0;
  thresh->lim = OP_ONE;
}

//-------------------------------------------------
//----- static function definition

static void op_thresh_update(op_thresh_t* thresh) {
  if(thresh->state < thresh->lim) {
    net_activate(thresh->outs[0], thresh->state, thresh);
  } else {
    net_activate(thresh->outs[1], thresh->state, thresh);
  }
}

//===== operator input


// input multiplier
static void op_thresh_in_state(op_thresh_t* thresh, const io_t v) {
  thresh->state = (v);
  op_thresh_update(thresh);
}

// input threshold
static void op_thresh_in_lim(op_thresh_t* thresh, const io_t v) {
  thresh->lim = v;
  op_thresh_update(thresh);

}


// serialization
u8* op_thresh_pickle(op_thresh_t* thresh, u8* dst) {
  // store state variables
  dst = pickle_io(thresh->state, dst);
  dst = pickle_io(thresh->lim, dst);
  return dst;
}

u8* op_thresh_unpickle(op_thresh_t* thresh, const u8* src) {
  // retreive state variables
  src = unpickle_io(src, (u32*)&(thresh->state));
  src = unpickle_io(src, (u32*)&(thresh->lim));
  return (u8*)src;
}
