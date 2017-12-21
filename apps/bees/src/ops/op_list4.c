#include "net_protected.h"
#include "op_list4.h"

//-------------------------------------------------
//---- static func declare
static void op_list4_in_index(op_list4_t* add, const io_t v);
static void op_list4_in_i0(op_list4_t* add, const io_t v);
static void op_list4_in_i1(op_list4_t* add, const io_t v);
static void op_list4_in_i2(op_list4_t* add, const io_t v);
static void op_list4_in_i3(op_list4_t* add, const io_t v);

// pickle / unpickle
static u8* op_list4_pickle(op_list4_t* op, u8* dst);
static const u8* op_list4_unpickle(op_list4_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_list4_instring = "INDEX\0  I0\0     I1\0     I2\0     I3\0     ";
static const char* op_list4_outstring = "VAL\0    ";
static const char* op_list4_opstring = "LIST4";

static op_in_fn op_list4_in_fn[5] = {
  (op_in_fn)&op_list4_in_index,
  (op_in_fn)&op_list4_in_i0,
  (op_in_fn)&op_list4_in_i1,
  (op_in_fn)&op_list4_in_i2,
  (op_in_fn)&op_list4_in_i3
};

//-------------------------------------------------
//---- external func define
void op_list4_init(void* mem) {
  op_list4_t* list4 = (op_list4_t*)mem;
  list4->super.numInputs = 5;
  list4->super.numOutputs = 1;
  list4->outs[0] = -1;

  list4->super.in_fn = op_list4_in_fn;
  list4->super.in_val = list4->in_val;
  list4->super.pickle = (op_pickle_fn) (&op_list4_pickle);
  list4->super.unpickle = (op_unpickle_fn) (&op_list4_unpickle);

  list4->super.out = list4->outs;
  list4->super.opString = op_list4_opstring;
  list4->super.inString = op_list4_instring;
  list4->super.outString = op_list4_outstring;
  list4->super.type = eOpList4;  

  list4->in_val[0] = &(list4->index);
  list4->in_val[1] = &(list4->i0);
  list4->in_val[2] = &(list4->i1);
  list4->in_val[3] = &(list4->i2);
  list4->in_val[4] = &(list4->i3);

  list4->index = 0;
  list4->i0 = 0;
  list4->i1 = 0;
  list4->i2 = 0;
  list4->i3 = 0;
}

//-------------------------------------------------
//---- static func define

static void op_list4_in_index(op_list4_t* list4, const io_t val) {
  io_t v = val;
  if(v<0) v = 0;
  if(v>3) v = 3;
  list4->index = v;

  switch(v) {
    case 0:
      list4->val = list4->i0;
      break;
    case 1:
      list4->val = list4->i1;
      break;
    case 2:
      list4->val = list4->i2;
      break;
    case 3:
      list4->val = list4->i3;
      break;
  }

  net_activate(list4, 0, list4->val);
}

static void op_list4_in_i0(op_list4_t* list4, const io_t val) {
  list4->i0 = val;
}

static void op_list4_in_i1(op_list4_t* list4, const io_t val) {
  list4->i1 = val;
}

static void op_list4_in_i2(op_list4_t* list4, const io_t val) {
  list4->i2 = val;
}

static void op_list4_in_i3(op_list4_t* list4, const io_t val) {
  list4->i3 = val;
}

// pickle / unpickle
u8* op_list4_pickle(op_list4_t* op, u8* dst) {
  dst = pickle_io(op->index, dst);
  dst = pickle_io(op->i0, dst);
  dst = pickle_io(op->i1, dst);
  dst = pickle_io(op->i2, dst);
  dst = pickle_io(op->i3, dst);

  return dst;
}

const u8* op_list4_unpickle(op_list4_t* op, const u8* src) {
  src = unpickle_io(src, &(op->index));
  src = unpickle_io(src, &(op->i0));
  src = unpickle_io(src, &(op->i1));
  src = unpickle_io(src, &(op->i2));
  src = unpickle_io(src, &(op->i3));

  return src;
}
