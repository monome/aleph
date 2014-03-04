#include "net_protected.h"
#include "op_add.h"

//-------------------------------------------------
//---- static func declare
static void op_add_in_a(op_add_t* add, const io_t v);
static void op_add_in_b(op_add_t* add, const io_t v);
static void op_add_in_btrig(op_add_t* add, const io_t v);

// pickle / unpickle
static u8* op_add_pickle(op_add_t* op, u8* dst);
static const u8* op_add_unpickle(op_add_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_add_instring = "A\0      B\0      TRIG\0   ";
static const char* op_add_outstring = "SUM\0    ";
static const char* op_add_opstring = "ADD";

static op_in_fn op_add_in_fn[3] = {
  (op_in_fn)&op_add_in_a,
  (op_in_fn)&op_add_in_b, 
  (op_in_fn)&op_add_in_btrig
};

//-------------------------------------------------
//---- external func define
void op_add_init(void* mem) {
  op_add_t* add = (op_add_t*)mem;
  add->super.numInputs = 3;
  add->super.numOutputs = 1;
  add->outs[0] = -1;

  add->super.in_fn = op_add_in_fn;
  add->super.in_val = add->in_val;
  add->super.pickle = (op_pickle_fn) (&op_add_pickle);
  add->super.unpickle = (op_unpickle_fn) (&op_add_unpickle);

  add->super.out = add->outs;
  add->super.opString = op_add_opstring;
  add->super.inString = op_add_instring;
  add->super.outString = op_add_outstring;
  add->super.type = eOpAdd;  

  add->in_val[0] = &(add->a);
  add->in_val[1] = &(add->b);
  add->in_val[2] = &(add->btrig);

  add->a = 0;
  add->b = 0;
  add->btrig = 0;

}

//-------------------------------------------------
//---- static func define

static void op_add_in_a(op_add_t* add, const io_t v) {
  add->a = v;
  add->val = op_sadd(add->a, add->b);
  net_activate(add->outs[0], add->val, add);
}

static void op_add_in_b(op_add_t* add, const io_t v) {
  add->b = v;
  add->val = op_sadd(add->a, add->b);
  if(add->btrig) {
    net_activate(add->outs[0], add->val, add);
  }
}

static void op_add_in_btrig(op_add_t* add, const io_t v) {
  if(v > 0) { add->btrig = OP_ONE; } else { add->btrig = 0; }
}


// pickle / unpickle
u8* op_add_pickle(op_add_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_add_unpickle(op_add_t* op, const u8* src) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
