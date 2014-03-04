#include "net_protected.h"
#include "op_list2.h"

//-------------------------------------------------
//---- static func declare
static void op_list2_in_index(op_list2_t* add, const io_t v);
static void op_list2_in_i0(op_list2_t* add, const io_t v);
static void op_list2_in_i1(op_list2_t* add, const io_t v);

// pickle / unpickle
static u8* op_list2_pickle(op_list2_t* op, u8* dst);
static const u8* op_list2_unpickle(op_list2_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_list2_instring = "INDEX\0  I0\0     I1\0     ";
static const char* op_list2_outstring = "VAL\0    ";
static const char* op_list2_opstring = "LIST2";

static op_in_fn op_list2_in_fn[3] = {
  (op_in_fn)&op_list2_in_index,
  (op_in_fn)&op_list2_in_i0,
  (op_in_fn)&op_list2_in_i1
};

//-------------------------------------------------
//---- external func define
void op_list2_init(void* mem) {
  op_list2_t* list2 = (op_list2_t*)mem;
  list2->super.numInputs = 3;
  list2->super.numOutputs = 1;
  list2->outs[0] = -1;

  list2->super.in_fn = op_list2_in_fn;
  list2->super.in_val = list2->in_val;
  list2->super.pickle = (op_pickle_fn) (&op_list2_pickle);
  list2->super.unpickle = (op_unpickle_fn) (&op_list2_unpickle);

  list2->super.out = list2->outs;
  list2->super.opString = op_list2_opstring;
  list2->super.inString = op_list2_instring;
  list2->super.outString = op_list2_outstring;
  list2->super.type = eOpList2;  

  list2->in_val[0] = &(list2->index);
  list2->in_val[1] = &(list2->i0);
  list2->in_val[2] = &(list2->i1);

  list2->index = 0;
  list2->i0 = 0;
  list2->i1 = 0;
}

//-------------------------------------------------
//---- static func define

static void op_list2_in_index(op_list2_t* list2, const io_t val) {
  io_t v = val;
  if(v<0) v = 0;
  if(v>1) v = 1;
  list2->index = v;

  switch(v) {
    case 0:
      list2->val = list2->i0;
      break;
    case 1:
      list2->val = list2->i1;
      break;
  }

  net_activate(list2->outs[0], list2->val, list2);
}

static void op_list2_in_i0(op_list2_t* list2, const io_t val) {
  list2->i0 = val;
}

static void op_list2_in_i1(op_list2_t* list2, const io_t val) {
  list2->i1 = val;
}

// pickle / unpickle
u8* op_list2_pickle(op_list2_t* op, u8* dst) {
  dst = pickle_io(op->index, dst);
  dst = pickle_io(op->i0, dst);
  dst = pickle_io(op->i1, dst);

  return dst;
}

const u8* op_list2_unpickle(op_list2_t* op, const u8* src) {
  src = unpickle_io(src, &(op->index));
  src = unpickle_io(src, &(op->i0));
  src = unpickle_io(src, &(op->i1));

  return src;
}
