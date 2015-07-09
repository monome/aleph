#include "net_protected.h"
#include "op_change.h"

//-------------------------------------------------
//----- static function declaration
static void op_change_in_val(op_change_t* op, const io_t v);
static void op_change_in_last(op_change_t* op, const io_t v);

// pickle / unpickle
static u8* op_change_pickle(op_change_t* op, u8* dst);
static const u8* op_change_unpickle(op_change_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_change_in_fn[2] = {
  (op_in_fn)&op_change_in_val,
  (op_in_fn)&op_change_in_last,
};

static const char* op_change_instring  = "VAL\0    LAST\0   ";
static const char* op_change_outstring = "VAL\0    ";
static const char* op_change_opstring  = "CHANGE";

//-------------------------------------------------
//----- external function definitions
void op_change_init(void* mem) {
  op_change_t* op = (op_change_t*)mem;
  op->super.numInputs = 2;
  op->super.numOutputs = 1;
  op->outs[0] = -1;

  op->super.in_fn = op_change_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_change_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_change_unpickle);

  op->super.in_val = op->in_val;
  op->super.out = op->outs;
  op->super.opString = op_change_opstring;
  op->super.inString = op_change_instring;
  op->super.outString = op_change_outstring;
  op->super.type = eOpChange;
  op->in_val[0] = &(op->val);
  op->in_val[1] = &(op->last);

  op->val = 0;
  op->last = 0;
}

//-------------------------------------------------
//----- static function definitions
static void op_change_in_val(op_change_t* op, const io_t v) {
  op->val = v;
  if (v != op->last) {
    op->last = op->val;
    net_activate(op->outs[0], op->val, op);
  }
}

static void op_change_in_last(op_change_t* op, const io_t v) {
  op->last = v;
}

// pickle / unpickle
u8* op_change_pickle(op_change_t* op, u8* dst) {
  dst = pickle_io(op->last, dst);
  dst = pickle_io(op->val, dst);
  return dst;
}

const u8* op_change_unpickle(op_change_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->last));
  src = unpickle_io(src, &(op->val));
  return src;
}
