#include "net_protected.h"
#include "op_mul.h"

//-------------------------------------------------
//----- static function declaration
static void op_mul_inc_input(op_mul_t* mul, const s16 idx, const io_t inc);
static void op_mul_in_a(op_mul_t* mul, const io_t v);
static void op_mul_in_b(op_mul_t* mul, const io_t v);
static void op_mul_in_btrig(op_mul_t* mul, const io_t v);

// pickle / unpickle
static u8* op_mul_pickle(op_mul_t* op, u8* dst);
static const u8* op_mul_unpickle(op_mul_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_mul_in_fn[3] = {
  (op_in_fn)&op_mul_in_a,
  (op_in_fn)&op_mul_in_b,
  (op_in_fn)&op_mul_in_btrig
};

static const char* op_mul_instring  = "A       B       B_TRIG  ";
static const char* op_mul_outstring = "VAL     ";
static const char* op_mul_opstring  = "MUL";

//-------------------------------------------------
//----- external function definitions
void op_mul_init(void* mem) {
  op_mul_t* mul = (op_mul_t*)mem;
  mul->super.numInputs = 3;
  mul->super.numOutputs = 1;
  mul->outs[0] = -1;

  mul->super.inc_fn = (op_inc_fn)op_mul_inc_input;
  mul->super.in_fn = op_mul_in_fn;
  mul->super.pickle = (op_pickle_fn) (&op_mul_pickle);
  mul->super.unpickle = (op_unpickle_fn) (&op_mul_unpickle);

  mul->super.in_val = mul->in_val;
  mul->super.out = mul->outs;
  mul->super.opString = op_mul_opstring;
  mul->super.inString = op_mul_instring;
  mul->super.outString = op_mul_outstring;
  mul->super.type = eOpMul;  
  mul->in_val[0] = &(mul->a);
  mul->in_val[1] = &(mul->b);
  mul->in_val[2] = &(mul->btrig);
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_mul_in_a(op_mul_t* mul, const io_t v) {
  // printf("mul at %d received A %d\n", (int)mul, (int)*v);
  mul->a = v;
  mul->val = OP_MUL(mul->a, mul->b);
  net_activate(mul->outs[0], mul->val, mul);
}

// set operand B
static void op_mul_in_b(op_mul_t* mul, const io_t v) {
  //printf("mul at %d received B %d\n", (int)mul, (int)*v);
  mul->b = v;
  mul->val = OP_MUL(mul->a, mul->b);
  if(mul->btrig) {
    net_activate(mul->outs[0], mul->val, mul);
  }
}

// set b-trigger mode
static void op_mul_in_btrig(op_mul_t* mul, const io_t v) {
  //printf("mul at %d received BTRIG %d\n", (int)mul, (int)*v);
  if(v > 0) { mul->btrig = OP_ONE; } { mul->btrig = 0; }
}

//===== UI input
static void op_mul_inc_input(op_mul_t* mul, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0:  // a
    val = OP_ADD(mul->a, inc);
    op_mul_in_a(mul, val);
    break; 
  case 1:  // b
    val = OP_ADD(mul->b, inc);
    op_mul_in_b(mul, val);
    break;
  case 2:  // trig
    op_mul_in_btrig(mul, inc);
    break;
  }
}

// pickle / unpickle
u8* op_mul_pickle(op_mul_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_mul_unpickle(op_mul_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
