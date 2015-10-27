#include "net_protected.h"
#include "op_shr.h"

//-------------------------------------------------
//----- static function declaration
static void op_shr_in_a(op_shr_t* shr, const io_t v);
static void op_shr_in_b(op_shr_t* shr, const io_t v);
static void op_shr_in_btrig(op_shr_t* shr, const io_t v);

// pickle / unpickle
static u8* op_shr_pickle(op_shr_t* op, u8* dst);
static const u8* op_shr_unpickle(op_shr_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_shr_in_fn[3] = {
  (op_in_fn)&op_shr_in_a,
  (op_in_fn)&op_shr_in_b,
  (op_in_fn)&op_shr_in_btrig
};

static const char* op_shr_instring  = "A\0      B\0      TRIG\0   ";
static const char* op_shr_outstring = "VAL\0    ";
static const char* op_shr_opstring  = "SHR";

//-------------------------------------------------
//----- external function definitions
void op_shr_init(void* mem) {
  op_shr_t* shr = (op_shr_t*)mem;
  shr->super.numInputs = 3;
  shr->super.numOutputs = 1;
  shr->outs[0] = -1;

  shr->super.in_fn = op_shr_in_fn;
  shr->super.pickle = (op_pickle_fn) (&op_shr_pickle);
  shr->super.unpickle = (op_unpickle_fn) (&op_shr_unpickle);

  shr->super.in_val = shr->in_val;
  shr->super.out = shr->outs;
  shr->super.opString = op_shr_opstring;
  shr->super.inString = op_shr_instring;
  shr->super.outString = op_shr_outstring;
  shr->super.type = eOpShr;  

  shr->in_val[0] = &(shr->a);
  shr->in_val[1] = &(shr->b);
  shr->in_val[2] = &(shr->btrig);

  shr->a = 0;
  shr->b = 1;
  shr->btrig = 0;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_shr_in_a(op_shr_t* shr, const io_t v) {
  // printf("shr at %d received A %d\n", (int)shr, (int)*v);
  shr->a = v;
  shr->val = shr->a >> shr->b;
  net_activate(shr->outs[0], shr->val, shr);
}

// set operand B
static void op_shr_in_b(op_shr_t* shr, const io_t v) {
  //printf("shr at %d received B %d\n", (int)shr, (int)*v);
  shr->b = v;
  shr->val = shr->a >> shr->b;
  if(shr->btrig) {
    net_activate(shr->outs[0], shr->val, shr);
  }
}

// set b-trigger mode
static void op_shr_in_btrig(op_shr_t* shr, const io_t v) {
  //printf("shr at %d received BTRIG %d\n", (int)shr, (int)*v);
  if(v > 0) { shr->btrig = OP_ONE; } else { shr->btrig = 0; }
}


// pickle / unpickle
u8* op_shr_pickle(op_shr_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_shr_unpickle(op_shr_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
