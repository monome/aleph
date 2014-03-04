#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_accum.h"

//-------------------------------------------------
//----- descriptor
static const char* op_accum_instring =  "INC\0    VAL\0    MIN\0    MAX\0    WRAP\0   ";
static const char* op_accum_outstring = "VAL\0    WRAP\0   ";
static const char* op_accum_opstring =  "ACCUM";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_accum_in_inc(op_accum_t* accum, const io_t v);
static void op_accum_in_val(op_accum_t* accum, const io_t v);
static void op_accum_in_min(op_accum_t* accum, const io_t );
static void op_accum_in_max(op_accum_t* accum, const io_t );
static void op_accum_in_wrap(op_accum_t* accum, const io_t );
// pickle / unpickle
static u8* op_accum_pickle(op_accum_t* accum, u8* dst);
static u8* op_accum_unpickle(op_accum_t* accum, const u8* src);
// wrap and output
static void op_accum_wrap_out(op_accum_t* accum);

// array of input functions 
static op_in_fn op_accum_in[5] = {
  (op_in_fn)&op_accum_in_inc,
  (op_in_fn)&op_accum_in_val,
  (op_in_fn)&op_accum_in_min,
  (op_in_fn)&op_accum_in_max,
  (op_in_fn)&op_accum_in_wrap
};

//----- external function definition

/// initialize
void op_accum_init(void* op) {
  op_accum_t* accum = (op_accum_t*)op;

  // superclass functions
  accum->super.in_fn = op_accum_in;
  accum->super.pickle = (op_pickle_fn) (&op_accum_pickle);
  accum->super.unpickle = (op_unpickle_fn) (&op_accum_unpickle);
  
  // superclass state
  accum->super.numInputs = 5;
  accum->super.numOutputs = 2;
  accum->outs[0] = -1;
  accum->outs[1] = -1;
 
  accum->super.in_val = accum->in_val;
  accum->in_val[0] = &(accum->inc);
  accum->in_val[1] = &(accum->val);
  accum->in_val[2] = &(accum->min);
  accum->in_val[3] = &(accum->max);
  accum->in_val[4] = &(accum->wrap);

  accum->super.out = accum->outs;
  accum->super.opString = op_accum_opstring;
  accum->super.inString = op_accum_instring;
  accum->super.outString = op_accum_outstring;
  accum->super.type = eOpAccum;

  // class state
  accum->inc = OP_ONE;
  accum->val = 0;
  accum->min = 0;
  accum->max = OP_MAX_VAL >> 1;
  accum->wrap = 1;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input increment
static void op_accum_in_inc(op_accum_t* accum, const io_t v) { 
  accum->inc = v;
  accum->val = op_sadd(accum->val, v);
  op_accum_wrap_out(accum);
}

// input value directly
static void op_accum_in_val(op_accum_t* accum, const io_t v) {
  //  if ((v) > 0) { accum->accum = OP_ONE; } else  { accum->accum = 0; } 
  accum->val = v;
  op_accum_wrap_out(accum);
}

// minimum
static void op_accum_in_min(op_accum_t* accum, const io_t v) {
  accum->min = v;
  if(accum->min >= accum->max) {
    accum->min = accum->max - 1;
  }
  op_accum_wrap_out(accum);
}

// maximum
static void op_accum_in_max(op_accum_t* accum, const io_t v) {
  accum->max = v;
  if(accum->min >= accum->max) {
    accum->max = accum->min + 1;
  }
  op_accum_wrap_out(accum);
}

// wrap mode
static void op_accum_in_wrap(op_accum_t* accum, const io_t v) {
  if(v > 0) { accum->wrap = OP_ONE; } else { accum->wrap = 0; }
}


// wrap and output
void op_accum_wrap_out(op_accum_t* accum) {
  io_t wrap = 0;
  io_t dif = 0;

  if (accum->wrap) { // wrapping...
    // if value needs wrapping, output the applied difference
    while (accum->val > accum->max) { 
      dif = op_sub(accum->min, accum->max) - 1;
      wrap = op_add(wrap, dif);
      accum->val = op_add(accum->val, dif);
    }
    while (accum->val < accum->min) { 
      dif = op_sub(accum->max, accum->min) + 1;
      wrap = op_add(wrap, dif);
      accum->val = op_add(accum->val, dif);
    }
  } else { // saturating...
    if (accum->val > accum->max) {
      wrap = dif = op_sub(accum->val, accum->max);
      accum->val = accum->max;
    }
    if (accum->val < accum->min) {
      wrap = dif = op_sub(accum->val , accum->min);
      accum->val = accum->min;
    }
  }

  // output the value
  net_activate(accum->outs[0], accum->val, accum);

  // output the wrap amount
  if (dif != 0) {
    net_activate(accum->outs[1], wrap, accum);  
  }
}

// serialization
u8* op_accum_pickle(op_accum_t* accum, u8* dst) {
  // store state variables
  dst = pickle_io(accum->val, dst);
  dst = pickle_io(accum->min, dst);
  dst = pickle_io(accum->max, dst);
  dst = pickle_io(accum->wrap, dst);
  return dst;
}

u8* op_accum_unpickle(op_accum_t* accum, const u8* src) {
  // retreive state variables
  src = unpickle_io(src, (u32*)&(accum->val));
  src = unpickle_io(src, (u32*)&(accum->min));
  src = unpickle_io(src, (u32*)&(accum->max));
  src = unpickle_io(src, (u32*)&(accum->wrap));
  return (u8*)src;
}
