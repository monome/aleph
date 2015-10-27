#include "net_protected.h"
#include "op_shl.h"

//-------------------------------------------------
//----- static function declaration
static void op_shl_in_a(op_shl_t* shl, const io_t v);
static void op_shl_in_b(op_shl_t* shl, const io_t v);
static void op_shl_in_btrig(op_shl_t* shl, const io_t v);

// pickle / unpickle
static u8* op_shl_pickle(op_shl_t* op, u8* dst);
static const u8* op_shl_unpickle(op_shl_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_shl_in_fn[3] = {
  (op_in_fn)&op_shl_in_a,
  (op_in_fn)&op_shl_in_b,
  (op_in_fn)&op_shl_in_btrig
};

static const char* op_shl_instring  = "A\0      B\0      TRIG\0   ";
static const char* op_shl_outstring = "VAL\0    ";
static const char* op_shl_opstring  = "SHL";

//-------------------------------------------------
//----- external function definitions
void op_shl_init(void* mem) {
  op_shl_t* shl = (op_shl_t*)mem;
  shl->super.numInputs = 3;
  shl->super.numOutputs = 1;
  shl->outs[0] = -1;

  shl->super.in_fn = op_shl_in_fn;
  shl->super.pickle = (op_pickle_fn) (&op_shl_pickle);
  shl->super.unpickle = (op_unpickle_fn) (&op_shl_unpickle);

  shl->super.in_val = shl->in_val;
  shl->super.out = shl->outs;
  shl->super.opString = op_shl_opstring;
  shl->super.inString = op_shl_instring;
  shl->super.outString = op_shl_outstring;
  shl->super.type = eOpShl;  

  shl->in_val[0] = &(shl->a);
  shl->in_val[1] = &(shl->b);
  shl->in_val[2] = &(shl->btrig);

  shl->a = 0;
  shl->b = 1;
  shl->btrig = 0;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_shl_in_a(op_shl_t* shl, const io_t v) {
  // printf("shl at %d received A %d\n", (int)shl, (int)*v);
  shl->a = v;
  shl->val = shl->a << shl->b;
  net_activate(shl->outs[0], shl->val, shl);
}

// set operand B
static void op_shl_in_b(op_shl_t* shl, const io_t v) {
  //printf("shl at %d received B %d\n", (int)shl, (int)*v);
  shl->b = v;
  shl->val = shl->a << shl->b;
  if(shl->btrig) {
    net_activate(shl->outs[0], shl->val, shl);
  }
}

// set b-trigger mode
static void op_shl_in_btrig(op_shl_t* shl, const io_t v) {
  //printf("shl at %d received BTRIG %d\n", (int)shl, (int)*v);
  if(v > 0) { shl->btrig = OP_ONE; } else { shl->btrig = 0; }
}


// pickle / unpickle
u8* op_shl_pickle(op_shl_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_shl_unpickle(op_shl_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
