#include "net_protected.h"
#include "op_list8.h"

//-------------------------------------------------
//---- static func declare
static void op_list8_in_index(op_list8_t* add, const io_t v);
static void op_list8_in_i0(op_list8_t* add, const io_t v);
static void op_list8_in_i1(op_list8_t* add, const io_t v);
static void op_list8_in_i2(op_list8_t* add, const io_t v);
static void op_list8_in_i3(op_list8_t* add, const io_t v);
static void op_list8_in_i4(op_list8_t* add, const io_t v);
static void op_list8_in_i5(op_list8_t* add, const io_t v);
static void op_list8_in_i6(op_list8_t* add, const io_t v);
static void op_list8_in_i7(op_list8_t* add, const io_t v);

// pickle / unpickle
static u8* op_list8_pickle(op_list8_t* op, u8* dst);
static const u8* op_list8_unpickle(op_list8_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_list8_instring = "INDEX\0  I0\0     I1\0     I2\0     I3\0     I4\0     I5\0     I6\0     I7\0     ";
static const char* op_list8_outstring = "VAL\0    ";
static const char* op_list8_opstring = "LIST8";

static op_in_fn op_list8_in_fn[9] = {
  (op_in_fn)&op_list8_in_index,
  (op_in_fn)&op_list8_in_i0,
  (op_in_fn)&op_list8_in_i1,
  (op_in_fn)&op_list8_in_i2,
  (op_in_fn)&op_list8_in_i3,
  (op_in_fn)&op_list8_in_i4,
  (op_in_fn)&op_list8_in_i5,
  (op_in_fn)&op_list8_in_i6,
  (op_in_fn)&op_list8_in_i7
};

//-------------------------------------------------
//---- external func define
void op_list8_init(void* mem) {
  op_list8_t* list8 = (op_list8_t*)mem;
  list8->super.numInputs = 9;
  list8->super.numOutputs = 1;
  list8->outs[0] = -1;

  list8->super.in_fn = op_list8_in_fn;
  list8->super.in_val = list8->in_val;
  list8->super.pickle = (op_pickle_fn) (&op_list8_pickle);
  list8->super.unpickle = (op_unpickle_fn) (&op_list8_unpickle);

  list8->super.out = list8->outs;
  list8->super.opString = op_list8_opstring;
  list8->super.inString = op_list8_instring;
  list8->super.outString = op_list8_outstring;
  list8->super.type = eOpList8;  

  list8->in_val[0] = &(list8->index);
  list8->in_val[1] = &(list8->i0);
  list8->in_val[2] = &(list8->i1);
  list8->in_val[3] = &(list8->i2);
  list8->in_val[4] = &(list8->i3);
  list8->in_val[5] = &(list8->i4);
  list8->in_val[6] = &(list8->i5);
  list8->in_val[7] = &(list8->i6);
  list8->in_val[8] = &(list8->i7);

  list8->index = 0;
  list8->i0 = 0;
  list8->i1 = 0;
  list8->i2 = 0;
  list8->i3 = 0;
  list8->i4 = 0;
  list8->i5 = 0;
  list8->i6 = 0;
  list8->i7 = 0;
}

//-------------------------------------------------
//---- static func define

static void op_list8_in_index(op_list8_t* list8, const io_t val) {
  io_t v = val;
  if(v<0) v = 0;
  if(v>7) v = 7;
  list8->index = v;

  switch(v) {
    case 0:
      list8->val = list8->i0;
      break;
    case 1:
      list8->val = list8->i1;
      break;
    case 2:
      list8->val = list8->i2;
      break;
    case 3:
      list8->val = list8->i3;
      break;
    case 4:
      list8->val = list8->i4;
      break;
    case 5:
      list8->val = list8->i5;
      break;
    case 6:
      list8->val = list8->i6;
      break;
    case 7:
      list8->val = list8->i7;
      break;
  }

  net_activate(list8->outs[0], list8->val, list8);
}

static void op_list8_in_i0(op_list8_t* list8, const io_t val) {
  list8->i0 = val;
}

static void op_list8_in_i1(op_list8_t* list8, const io_t val) {
  list8->i1 = val;
}

static void op_list8_in_i2(op_list8_t* list8, const io_t val) {
  list8->i2 = val;
}

static void op_list8_in_i3(op_list8_t* list8, const io_t val) {
  list8->i3 = val;
}

static void op_list8_in_i4(op_list8_t* list8, const io_t val) {
  list8->i4 = val;
}

static void op_list8_in_i5(op_list8_t* list8, const io_t val) {
  list8->i5 = val;
}

static void op_list8_in_i6(op_list8_t* list8, const io_t val) {
  list8->i6 = val;
}

static void op_list8_in_i7(op_list8_t* list8, const io_t val) {
  list8->i7 = val;
}


// pickle / unpickle
u8* op_list8_pickle(op_list8_t* op, u8* dst) {
  dst = pickle_io(op->index, dst);
  dst = pickle_io(op->i0, dst);
  dst = pickle_io(op->i1, dst);
  dst = pickle_io(op->i2, dst);
  dst = pickle_io(op->i3, dst);
  dst = pickle_io(op->i4, dst);
  dst = pickle_io(op->i5, dst);
  dst = pickle_io(op->i6, dst);
  dst = pickle_io(op->i7, dst);

  return dst;
}

const u8* op_list8_unpickle(op_list8_t* op, const u8* src) {
  src = unpickle_io(src, &(op->index));
  src = unpickle_io(src, &(op->i0));
  src = unpickle_io(src, &(op->i1));
  src = unpickle_io(src, &(op->i2));
  src = unpickle_io(src, &(op->i3));
  src = unpickle_io(src, &(op->i4));
  src = unpickle_io(src, &(op->i5));
  src = unpickle_io(src, &(op->i6));
  src = unpickle_io(src, &(op->i7));

  return src;
}
