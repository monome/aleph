// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_enc.h"
#include "pickle.h"

//-------------------------------------------------
//----- static function declarations
static void op_enc_inc_input ( op_enc_t* enc, const s16 idx, const io_t inc);
static void op_enc_perform   ( op_enc_t* enc) ;
static void op_enc_in_wrap   ( op_enc_t* enc, const io_t v);
static void op_enc_in_max    ( op_enc_t* enc, const io_t v);
static void op_enc_in_min    ( op_enc_t* enc, const io_t v);
//static void op_enc_in_move   ( op_enc_t* enc, const io_t v);
static void op_enc_in_step   ( op_enc_t* enc, const io_t v);

// pickles
static u8* op_enc_pickle	(op_enc_t* enc, u8* dst);
static const u8* op_enc_unpickle(op_enc_t* enc, const u8* src);


//-------------------------------------------------
//----- static vars
static const char* op_enc_instring  = "MIN     MAX     STEP    WRAP    ";
static const char* op_enc_outstring = "VAL     WRAP    ";
static const char* op_enc_opstring  = "ENC";
static void op_enc_perform(op_enc_t* enc);

// input function pointers
static op_in_fn op_enc_in_fn[4] = {
  //  (op_in_fn)&op_enc_in_move,
  (op_in_fn)&op_enc_in_min,
  (op_in_fn)&op_enc_in_max,
  (op_in_fn)&op_enc_in_step,
  (op_in_fn)&op_enc_in_wrap,
};

//-------------------------------------------------
//----- external functions
// initialize
void op_enc_init(void* mem) {
  op_enc_t* enc = (op_enc_t*)mem;

  // superclass functions
  enc->super.inc_fn = (op_inc_fn)(&op_enc_inc_input);
  enc->super.in_fn = op_enc_in_fn;
  enc->super.pickle = (op_pickle_fn) (&op_enc_pickle);
  enc->super.unpickle = (op_unpickle_fn) (&op_enc_unpickle);

  // superclass state
  enc->super.numInputs = 4;
  enc->super.numOutputs = 2;
  enc->outs[0] = -1;
  enc->outs[1] = -1;
  enc->super.in_val = enc->in_val;
  enc->super.out = enc->outs;
  enc->super.opString = op_enc_opstring;
  enc->super.inString = op_enc_instring;
  enc->super.outString = op_enc_outstring;
  enc->super.type = eOpEnc;
  enc->super.flags |= (1 << eOpFlagSys); // system 

  //  enc->in_val[0] = &(enc->move);
  enc->in_val[0] = &(enc->min);
  enc->in_val[1] = &(enc->max);
  enc->in_val[2] = &(enc->step);
  enc->in_val[3] = &(enc->wrap);
  
  enc->min = 0;
  enc->max = OP_MAX_VAL;
  enc->step = OP_MIN_INC;
  enc->wrap = 0;

}

//-------------------------------------------------
//----- static function definitions

//======= operator input
// step
static void op_enc_in_step(op_enc_t* enc, const io_t v) {
  enc->step = v;
  // op_enc_perform(enc);
}

// move
/* static void op_enc_in_move(op_enc_t* enc, const io_t v) { */
/*   enc->val = op_add(enc->val, op_mul(enc->step, op_from_int(v))); */
/*   op_enc_perform(enc); */
/* } */

// max
static void op_enc_in_min(op_enc_t* enc, const io_t v) {
  enc->min = v;
  op_enc_perform(enc);
}

// max
static void op_enc_in_max(op_enc_t* enc, const io_t v) {
  enc->max = v;
  op_enc_perform(enc);
}

// wrap behavior
static void op_enc_in_wrap(op_enc_t* enc, const io_t v) {
  //  enc->wrap = (*v > 0);
  if(v > 0) { enc->wrap = OP_ONE; } else { enc->wrap = 0; }
  //  op_enc_perform(enc);
}

// perform wrapping and output
static void op_enc_perform(op_enc_t* enc) { 
  io_t wrap = 0;
  io_t dif = 0;
  if (enc->wrap) { // wrapping...
    // if value needs wrapping, output the applied difference
    while (enc->val > enc->max) { 
      dif = op_sub(enc->min, enc->max);
      wrap = op_add(wrap, dif);
      enc->val = op_add(enc->val, dif);
    }
    while (enc->val < enc->min) { 
      dif = op_sub(enc->max, enc->min);
      wrap = op_add(wrap, dif);
      enc->val = op_add(enc->val, dif);
    }
  } else { // saturating...
    if (enc->val > enc->max) {
      enc->val = enc->max;
      dif = 1; // force wrap output
    }
    if (enc->val < enc->min) {
      enc->val = enc->min;
      dif = -1; // force wrap output
    }
  }

  // output the value
  net_activate(enc->outs[0], enc->val, enc);

  // output the wrap amount
  if (dif != 0) {
    net_activate(enc->outs[1], wrap, enc);  
  }
}

//===== UI input
static void op_enc_inc_input(op_enc_t* enc, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  /* case 0:  // move */
  /*   op_enc_in_move(enc, inc); */
  /*   break;  */
  case 0:  // min
    val = op_sadd(enc->min, inc);
    op_enc_in_min(enc, val);
    break;
  case 1:  // max
    val = op_sadd(enc->max, inc);
    op_enc_in_max(enc, val);
    break;
  case 2: // step
    val = op_sadd(enc->step, inc);
    op_enc_in_step(enc, val);
    break;
  case 3: // wrap mode
    op_enc_in_wrap(enc, inc);
    break;
  }
}

// pickles
u8* op_enc_pickle(op_enc_t* enc, u8* dst) {
  dst = pickle_io(enc->val, dst);
  dst = pickle_io(enc->step, dst);
  dst = pickle_io(enc->min, dst);
  dst = pickle_io(enc->max, dst);
  dst = pickle_io(enc->wrap, dst);
  return dst;
}

const u8* op_enc_unpickle(op_enc_t* enc, const u8* src) {
  src = unpickle_io(src, &(enc->val));
  src = unpickle_io(src, &(enc->step));
  src = unpickle_io(src, &(enc->min));
  src = unpickle_io(src, &(enc->max));
  src = unpickle_io(src, &(enc->wrap));
  return src;
}

void op_enc_sys_input(op_enc_t* enc, s8 v) {
  /* print_dbg("\r\n enc sys input, address: 0x"); */
  /* print_dbg_hex((u32)enc); */
  /* print_dbg(" , input value: 0x"); */
  /* print_dbg_hex((u32)v); */

  enc->val = op_add(enc->val, op_mul(enc->step, op_from_int(v)));
  op_enc_perform(enc);  
}
