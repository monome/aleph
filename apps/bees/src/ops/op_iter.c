#include "net_protected.h"
#include "op_iter.h"

//-------------------------------------------------
//---- static func declare
static void op_iter_in_times(op_iter_t* iter, const io_t v);
static void op_iter_in_val(op_iter_t* iter, const io_t v);
static void op_iter_in_stop(op_iter_t* iter, const io_t v);

// pickle / unpickle
static u8* op_iter_pickle(op_iter_t* op, u8* dst);
static const u8* op_iter_unpickle(op_iter_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_iter_instring  = "TIMES\0  VAL\0    STOP\0   ";
static const char* op_iter_outstring = "IDX\0    VAL\0    DONE\0   ";
static const char* op_iter_opstring = "ITER";

static op_in_fn op_iter_in_fn[3] = {
  (op_in_fn)&op_iter_in_times,
  (op_in_fn)&op_iter_in_val,
  (op_in_fn)&op_iter_in_stop
};

//-------------------------------------------------
//---- external func define
void op_iter_init(void* mem) {
  op_iter_t* iter = (op_iter_t*)mem;
  iter->super.numInputs = 3;
  iter->super.numOutputs = 3;
  iter->outs[0] = -1;
  iter->outs[1] = -1;
  iter->outs[2] = -1;

  iter->super.in_fn = op_iter_in_fn;
  iter->super.in_val = iter->in_val;
  iter->super.pickle = (op_pickle_fn) (&op_iter_pickle);
  iter->super.unpickle = (op_unpickle_fn) (&op_iter_unpickle);

  iter->super.out = iter->outs;
  iter->super.opString = op_iter_opstring;
  iter->super.inString = op_iter_instring;
  iter->super.outString = op_iter_outstring;
  iter->super.type = eOpIter;  

  iter->in_val[0] = &(iter->times);
  iter->in_val[1] = &(iter->val);
  iter->in_val[2] = &(iter->stop);

  iter->times = 0;
  iter->val = 0;
  iter->stop = 0;

}

//-------------------------------------------------
//---- static func define

static void op_iter_in_times(op_iter_t* iter, const io_t v) {
  if (v >= 0 && v <= 16)
    iter->times = v;
}

static void op_iter_in_val(op_iter_t* iter, const io_t v) {
  u8 i = 0;
  iter->val = v;
  iter->stop = 0;
  for (i=0; i < iter->times; i++) {
    if ( iter->stop == 1)
      return;
    net_activate(iter->outs[0], i, iter);
    net_activate(iter->outs[1], iter->val, iter);
  }
  net_activate(iter->outs[2], 1, iter);
}

static void op_iter_in_stop(op_iter_t* iter, const io_t v) {
  iter->stop = 1;
}


// pickle / unpickle
u8* op_iter_pickle(op_iter_t* op, u8* dst) {
  dst = pickle_io(op->times, dst);
  dst = pickle_io(op->val, dst);
  dst = pickle_io(op->stop, dst);
  return dst;
}

const u8* op_iter_unpickle(op_iter_t* op, const u8* src) {
  src = unpickle_io(src, &(op->times));
  src = unpickle_io(src, &(op->val));
  src = unpickle_io(src, &(op->stop));
  return src;
}
