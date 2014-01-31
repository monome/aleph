#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_num.h"


//-------------------------------------------------
//----- static vars

//-- descriptor
static const char* op_num_instring = "VAL     X       Y       ";
static const char* op_num_outstring = "";
static const char* op_num_opstring = "NUM";

//-- temp
// string buffer for number->text rendering
static char tmpStr[16];
// try sharing region data among instances...
static volatile u8 regData[OP_NUM_GFX_BYTES];

//-------------------------------------------------
//----- static function declaration


// UI increment
static void op_num_inc(op_num_t* num, const s16 idx, const io_t inc);
// set inputs
static void op_num_in_val(op_num_t* num, const io_t v);
static void op_num_in_x(op_num_t* num, const io_t x);
static void op_num_in_y(op_num_t* num, const io_t y);

// pickle / unpickle
static u8* op_num_pickle(op_num_t* num, u8* dst);
static u8* op_num_unpickle(op_num_t* num, const u8* src);

// array of input functions 
static op_in_fn op_num_in[3] = {
  (op_in_fn)&op_num_in_val,
  (op_in_fn)&op_num_in_x
  (op_in_fn)&op_num_in_y
};

//----- external function definition

/// initialize
void op_num_init(void* op) {
  op_num_t* num = (op_num_t*)op;

  // superclass functions
  num->super.inc_fn = (op_inc_fn)&op_num_inc;
  num->super.in_fn = op_num_in;
  num->super.pickle = (op_pickle_fn) (&op_num_pickle);
  num->super.unpickle = (op_unpickle_fn) (&op_num_unpickle);
  
  // superclass val
  num->super.numInputs = 3;
  num->super.numOutputs = 0;
 
  num->super.in_val = num->in_val;
  num->in_val[0] = &(num->val);
  num->in_val[1] = &(num->x);
  num->in_val[2] = &(num->y);

  num->super.out = num->outs;
  num->super.opString = op_num_opstring;
  num->super.inString = op_num_instring;
  num->super.outString = op_num_outstring;
  num->super.type = eOpNum;

  // class val
  num->val = 0;
  num->x = 0;
  num->y = 0;

  // init region
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input val
static void op_num_in_val(op_num_t* num, const io_t v) {
  // draw the region!
  
  region_string_aa(&bigtop, "_LPPR", 40, 12, 1);
  
}

// input x position
static void op_num_in_x(op_num_t* num, const io_t v) {
  num->mul = v;

}
// input y position
static void op_num_in_x(op_num_t* num, const io_t v) {
  num->mul = v;
}

//===== UI input

// increment
static void op_num_inc(op_num_t* num, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0: // current value
    val = op_sadd(num->mul, inc);
    op_num_in_val(num, val);
    break;
  case 1: // x position
    val = op_sadd(num->mul, inc);
    op_num_in_x(num, val);
    break;
  case 2: // y position
    val = op_sadd(num->mul, inc);
    op_num_in_x(num, val);
    break;
  }
}


// serialization
u8* op_num_pickle(op_num_t* num, u8* dst) {
  // store val variables
  dst = pickle_io(num->val, dst);
  dst = pickle_io(num->x, dst);
  dst = pickle_io(num->y, dst);
  return dst;
}

u8* op_num_unpickle(op_num_t* num, const u8* src) {
  // retreive val  variables
  src = unpickle_io(src, (u32*)&(num->val));
  src = unpickle_io(src, (u32*)&(num->x));
  src = unpickle_io(src, (u32*)&(num->y));
   return (u8*)src;
}
