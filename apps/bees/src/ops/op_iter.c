#include "net_protected.h"
#include "op_iter.h"

//-------------------------------------------------
//---- static func declare
static void op_iter_in_a(op_iter_t* iter, const io_t v);
static void op_iter_in_b(op_iter_t* iter, const io_t v);
static void op_iter_in_btrig(op_iter_t* iter, const io_t v);

// pickle / unpickle
static u8* op_iter_pickle(op_iter_t* op, u8* dst);
static const u8* op_iter_unpickle(op_iter_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_iter_instring = "A\0      B\0      TRIG\0   ";
static const char* op_iter_outstring = "SUM\0    ";
static const char* op_iter_opstring = "ITER";

static op_in_fn op_iter_in_fn[3] = {
  (op_in_fn)&op_iter_in_a,
  (op_in_fn)&op_iter_in_b, 
  (op_in_fn)&op_iter_in_btrig
};

//-------------------------------------------------
//---- external func define
void op_iter_init(void* mem) {
  op_iter_t* iter = (op_iter_t*)mem;
  iter->super.numInputs = 3;
  iter->super.numOutputs = 1;
  iter->outs[0] = -1;

  iter->super.in_fn = op_iter_in_fn;
  iter->super.in_val = iter->in_val;
  iter->super.pickle = (op_pickle_fn) (&op_iter_pickle);
  iter->super.unpickle = (op_unpickle_fn) (&op_iter_unpickle);

  iter->super.out = iter->outs;
  iter->super.opString = op_iter_opstring;
  iter->super.inString = op_iter_instring;
  iter->super.outString = op_iter_outstring;
  iter->super.type = eOpIter;  

  iter->in_val[0] = &(iter->a);
  iter->in_val[1] = &(iter->b);
  iter->in_val[2] = &(iter->btrig);

  iter->a = 0;
  iter->b = 0;
  iter->btrig = 0;

}

//-------------------------------------------------
//---- static func define

static void op_iter_in_a(op_iter_t* iter, const io_t v) {
  iter->a = v;
  net_activate(iter->outs[0], iter->val, iter);
}

static void op_iter_in_b(op_iter_t* iter, const io_t v) {
  iter->b = v;
  if(iter->btrig) {
    net_activate(iter->outs[0], iter->val, iter);
  }
}

static void op_iter_in_btrig(op_iter_t* iter, const io_t v) {
  if(v > 0) { iter->btrig = OP_ONE; } else { iter->btrig = 0; }
}


// pickle / unpickle
u8* op_iter_pickle(op_iter_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_iter_unpickle(op_iter_t* op, const u8* src) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
