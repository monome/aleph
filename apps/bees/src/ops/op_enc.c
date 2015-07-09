// asf
#include "print_funcs.h"

// bees
#include "net_protected.h"
#include "op_enc.h"
#include "pickle.h"

//-------------------------------------------------
//----- static function declarations
static void op_enc_perform   ( op_enc_t* enc) ;
static void op_enc_in_val   ( op_enc_t* enc, const io_t v);
static void op_enc_in_max    ( op_enc_t* enc, const io_t v);
static void op_enc_in_min    ( op_enc_t* enc, const io_t v);
//static void op_enc_in_move   ( op_enc_t* enc, const io_t v);
static void op_enc_in_step   ( op_enc_t* enc, const io_t v);

// pickles
static u8* op_enc_pickle	(op_enc_t* enc, u8* dst);
static const u8* op_enc_unpickle(op_enc_t* enc, const u8* src);


//-------------------------------------------------
//----- static vars
static const char* op_enc_instring  = "MIN\0    MAX\0    STEP\0   VAL\0    ";
static const char* op_enc_outstring = "VAL\0    DELTA\0  ";
static const char* op_enc_opstring  = "ENC";
static void op_enc_perform(op_enc_t* enc);

// input function pointers
static op_in_fn op_enc_in_fn[4] = {
  //  (op_in_fn)&op_enc_in_move,
  (op_in_fn)&op_enc_in_min,
  (op_in_fn)&op_enc_in_max,
  (op_in_fn)&op_enc_in_step,
  (op_in_fn)&op_enc_in_val,
};

//-------------------------------------------------
//----- external functions
// initialize
void op_enc_init(void* mem) {
  op_enc_t* enc = (op_enc_t*)mem;

  // superclass functions
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
  enc->in_val[3] = &(enc->val);
  
  enc->min = 0;
  enc->max = OP_MAX_VAL;
  enc->step = OP_MIN_INC;

}

//-------------------------------------------------
//----- static function definitions

//======= operator input
// step
static void op_enc_in_step(op_enc_t* enc, const io_t v) {
  enc->step = v;
  // op_enc_perform(enc);
}
// max
static void op_enc_in_min(op_enc_t* enc, const io_t v) {
  /// fixme: for now, i am banning this kind of pathological shit:
  if(v >= enc->max) { enc->min = enc->max - 1; }
  else { enc->min = v; }
  op_enc_perform(enc);
}

// max
static void op_enc_in_max(op_enc_t* enc, const io_t v) {
  if(v <= enc->min) { enc->max = enc->min + 1; }
  else { enc->max = v; }
  op_enc_perform(enc);
}

// wrap behavior
static void op_enc_in_val(op_enc_t* enc, const io_t v) {
  enc->val32 = v;
  op_enc_perform(enc);
}

// perform wrapping and output
static void op_enc_perform(op_enc_t* enc) { 
  // s32 wrap = 0;
  // s32 dif = 0;

  /// FIXME: this 32-bit business is pretty foul stuff.
  s32 min32;
  s32 max32;
  if(enc->min < 0) {
    min32 = (s32)(enc->min) | 0xffff0000;
  } else {
    min32 = (s32)(enc->min);
  }
  if(enc->max < 0) {
    max32 = (s32)(enc->max) | 0xffff0000;
  } else {
    max32 = (s32)(enc->max);
  }
  
  /* /\* print_dbg("\r\n calculating enc output... min: 0x"); *\/ */
  /* /\* print_dbg_hex(min32); *\/ */
  /* /\* print_dbg(" , max: "); *\/ */
  /* /\* print_dbg_hex(max32); *\/ */
  /* /\* print_dbg(" , val: "); *\/ */
  /* /\* print_dbg_hex(enc->val32); *\/ */

  #if 0
  if (enc->wrap) { // wrapping...
    // if value needs wrapping, output the applied difference
    while (enc->val32 > max32) {

      print_dbg(" ... wrapping high... ");

      dif = min32 - max32;

      if(dif == 0) { dif = -1; }
      /* print_dbg(" , dif: "); */
      /* print_dbg_hex(dif); */

      wrap += dif;
      enc->val32 = enc->val32 + dif;

      /* print_dbg(" , new val: "); */
      /* print_dbg_hex(enc->val32); */

    }
    while (enc->val32 < min32) { 
      /* print_dbg(" ... wrapping low... "); */
      
      dif = max32 - min32;
      if(dif == 0) { dif = 1; }

      /* print_dbg(" , dif: "); */
      /* print_dbg_hex(dif); */


      wrap += dif;
      enc->val32 = enc->val32 + dif;

  /* print_dbg(" , new val: "); */
  /* print_dbg_hex(enc->val32); */

    }
    enc->val = op_from_int(enc->val32);
  } else { // saturating...

    #endif

    if (enc->val32 > (s32)(enc->max)) {
      enc->val = enc->max;
      // dif = 1; // force wrap output
    }
    else if (enc->val32 < (s32)(enc->min)) {
      enc->val = enc->min;
      // dif = -1; // force wrap output
    } else {
      enc->val = op_from_int(enc->val32);
    }
  // }

  // output the value
  net_activate(enc->outs[0], enc->val, enc);

  // output the wrap amount
  // if (dif != 0) {
    // net_activate(enc->outs[1], op_from_int(wrap), enc);  
  // }
}


// pickles
u8* op_enc_pickle(op_enc_t* enc, u8* dst) {
  /// if we want to maintain 0.5.x scene compatibility...
  io_t dummy = 0;
  dst = pickle_io(enc->val, dst);
  dst = pickle_io(enc->step, dst);
  dst = pickle_io(enc->min, dst);
  dst = pickle_io(enc->max, dst);
  dst = pickle_io(dummy, dst);
  return dst;
}

const u8* op_enc_unpickle(op_enc_t* enc, const u8* src) {
  /// if we want to maintain 0.5.x scene compatibility...
  io_t dummy = 0;
  src = unpickle_io(src, &(enc->val));
  src = unpickle_io(src, &(enc->step));
  src = unpickle_io(src, &(enc->min));
  src = unpickle_io(src, &(enc->max));
  src = unpickle_io(src, &(dummy));
  return src;
}

void op_enc_sys_input(op_enc_t* enc, s8 v) {
  /* print_dbg("\r\n enc sys input, address: 0x"); */
  /* print_dbg_hex((u32)enc); */
  /* print_dbg(" , input value: 0x"); */
  /* print_dbg_hex((u32)v); */
  // use saturating add. have to explicitly check for "would-be overflow" in wrap mode with max val.
  //  enc->val = op_sadd(enc->val, op_mul(enc->step, op_from_int(v)));
  /// HACK: assuming the io_t is small enough t
  /// that we can void overflow by casting to 4 bytes.

  enc->val32 = (s32)(enc->val) + (s32)(op_mul(enc->step, op_from_int(v)));
  op_enc_perform(enc);
  net_activate(enc->outs[1], op_from_int(v), enc); 
}
