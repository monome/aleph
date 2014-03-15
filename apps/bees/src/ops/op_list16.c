#include "net_protected.h"
#include "op_list16.h"

//-------------------------------------------------
//---- static func declare
static void op_list16_in_index(op_list16_t* add, const io_t v);
static void op_list16_in_i0(op_list16_t* add, const io_t v);
static void op_list16_in_i1(op_list16_t* add, const io_t v);
static void op_list16_in_i2(op_list16_t* add, const io_t v);
static void op_list16_in_i3(op_list16_t* add, const io_t v);
static void op_list16_in_i4(op_list16_t* add, const io_t v);
static void op_list16_in_i5(op_list16_t* add, const io_t v);
static void op_list16_in_i6(op_list16_t* add, const io_t v);
static void op_list16_in_i7(op_list16_t* add, const io_t v);
static void op_list16_in_i8(op_list16_t* add, const io_t v);
static void op_list16_in_i9(op_list16_t* add, const io_t v);
static void op_list16_in_i10(op_list16_t* add, const io_t v);
static void op_list16_in_i11(op_list16_t* add, const io_t v);
static void op_list16_in_i12(op_list16_t* add, const io_t v);
static void op_list16_in_i13(op_list16_t* add, const io_t v);
static void op_list16_in_i14(op_list16_t* add, const io_t v);
static void op_list16_in_i15(op_list16_t* add, const io_t v);

//static void op_list16_inc_input(op_list16_t* mul, const s16 idx, const io_t inc);

// pickle / unpickle
static u8* op_list16_pickle(op_list16_t* op, u8* dst);
static const u8* op_list16_unpickle(op_list16_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_list16_instring = "INDEX\0  I0\0     I1\0     I2\0     I3\0     I4\0     I5\0     I6\0     I7\0     I8\0     I9\0     I10\0    I11\0    I12\0    I13\0    I14\0    I15\0    ";
static const char* op_list16_outstring = "VAL\0    ";
static const char* op_list16_opstring = "LIST16";

static op_in_fn op_list16_in_fn[17] = {
  (op_in_fn)&op_list16_in_index,
  (op_in_fn)&op_list16_in_i0,
  (op_in_fn)&op_list16_in_i1,
  (op_in_fn)&op_list16_in_i2,
  (op_in_fn)&op_list16_in_i3,
  (op_in_fn)&op_list16_in_i4,
  (op_in_fn)&op_list16_in_i5,
  (op_in_fn)&op_list16_in_i6,
  (op_in_fn)&op_list16_in_i7,
  (op_in_fn)&op_list16_in_i8,
  (op_in_fn)&op_list16_in_i9,
  (op_in_fn)&op_list16_in_i10,
  (op_in_fn)&op_list16_in_i11,
  (op_in_fn)&op_list16_in_i12,
  (op_in_fn)&op_list16_in_i13,
  (op_in_fn)&op_list16_in_i14,
  (op_in_fn)&op_list16_in_i15
};

//-------------------------------------------------
//---- external func define
void op_list16_init(void* mem) {
  op_list16_t* list16 = (op_list16_t*)mem;
  list16->super.numInputs = 17;
  list16->super.numOutputs = 1;
  list16->outs[0] = -1;

  list16->super.in_fn = op_list16_in_fn;
  list16->super.in_val = list16->in_val;
  list16->super.pickle = (op_pickle_fn) (&op_list16_pickle);
  list16->super.unpickle = (op_unpickle_fn) (&op_list16_unpickle);

  list16->super.out = list16->outs;
  list16->super.opString = op_list16_opstring;
  list16->super.inString = op_list16_instring;
  list16->super.outString = op_list16_outstring;
  list16->super.type = eOpList16;  

  list16->in_val[0] = &(list16->index);
  list16->in_val[1] = &(list16->i0);
  list16->in_val[2] = &(list16->i1);
  list16->in_val[3] = &(list16->i2);
  list16->in_val[4] = &(list16->i3);
  list16->in_val[5] = &(list16->i4);
  list16->in_val[6] = &(list16->i5);
  list16->in_val[7] = &(list16->i6);
  list16->in_val[8] = &(list16->i7);
  list16->in_val[9] = &(list16->i8);
  list16->in_val[10] = &(list16->i9);
  list16->in_val[11] = &(list16->i10);
  list16->in_val[12] = &(list16->i11);
  list16->in_val[13] = &(list16->i12);
  list16->in_val[14] = &(list16->i13);
  list16->in_val[15] = &(list16->i14);
  list16->in_val[16] = &(list16->i15);

  list16->index = 0;
  list16->i0 = 0;
  list16->i1 = 0;
  list16->i2 = 0;
  list16->i3 = 0;
  list16->i4 = 0;
  list16->i5 = 0;
  list16->i6 = 0;
  list16->i7 = 0;
  list16->i8 = 0;
  list16->i9 = 0;
  list16->i10 = 0;
  list16->i11 = 0;
  list16->i12 = 0;
  list16->i13 = 0;
  list16->i14 = 0;
  list16->i15 = 0;
}

//-------------------------------------------------
//---- static func define

static void op_list16_in_index(op_list16_t* list16, const io_t val) {
  io_t v = val;
  if(v<0) v = 0;
  if(v>15) v = 15;
  list16->index = v;

  list16->val = *(list16->in_val[v+1]);

  net_activate(list16->outs[0], list16->val, list16);
}

static void op_list16_in_i0(op_list16_t* list16, const io_t val) {
  list16->i0 = val;
}

static void op_list16_in_i1(op_list16_t* list16, const io_t val) {
  list16->i1 = val;
}

static void op_list16_in_i2(op_list16_t* list16, const io_t val) {
  list16->i2 = val;
}

static void op_list16_in_i3(op_list16_t* list16, const io_t val) {
  list16->i3 = val;
}

static void op_list16_in_i4(op_list16_t* list16, const io_t val) {
  list16->i4 = val;
}

static void op_list16_in_i5(op_list16_t* list16, const io_t val) {
  list16->i5 = val;
}

static void op_list16_in_i6(op_list16_t* list16, const io_t val) {
  list16->i6 = val;
}

static void op_list16_in_i7(op_list16_t* list16, const io_t val) {
  list16->i7 = val;
}

static void op_list16_in_i8(op_list16_t* list16, const io_t val) {
  list16->i8 = val;
}

static void op_list16_in_i9(op_list16_t* list16, const io_t val) {
  list16->i9 = val;
}

static void op_list16_in_i10(op_list16_t* list16, const io_t val) {
  list16->i10 = val;
}

static void op_list16_in_i11(op_list16_t* list16, const io_t val) {
  list16->i11 = val;
}

static void op_list16_in_i12(op_list16_t* list16, const io_t val) {
  list16->i12 = val;
}

static void op_list16_in_i13(op_list16_t* list16, const io_t val) {
  list16->i13 = val;
}

static void op_list16_in_i14(op_list16_t* list16, const io_t val) {
  list16->i14 = val;
}

static void op_list16_in_i15(op_list16_t* list16, const io_t val) {
  list16->i15 = val;
}


//===== UI input
/*
static void op_list16_inc_input(op_list16_t* list16, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0:  // index
    val = op_sadd(list16->index, inc);
    if(val<0) val = 0;
    if(val>15) val = 15;
    op_list16_in_index(list16, val);
    break; 
  case 1:
    val = op_sadd(list16->i0, inc);
    op_list16_in_i0(list16, val);
    break;
  case 2:
    val = op_sadd(list16->i1, inc);
    op_list16_in_i1(list16, val);
    break;
  case 3:
    val = op_sadd(list16->i2, inc);
    op_list16_in_i2(list16, val);
    break;
  case 4:
    val = op_sadd(list16->i3, inc);
    op_list16_in_i3(list16, val);
    break;
  case 5:
    val = op_sadd(list16->i4, inc);
    op_list16_in_i4(list16, val);
    break;
  case 6:
    val = op_sadd(list16->i5, inc);
    op_list16_in_i5(list16, val);
    break;
  case 7:
    val = op_sadd(list16->i6, inc);
    op_list16_in_i6(list16, val);
    break;
  case 8:
    val = op_sadd(list16->i7, inc);
    op_list16_in_i7(list16, val);
    break;
  case 9:
    val = op_sadd(list16->i8, inc);
    op_list16_in_i8(list16, val);
    break;
  case 10:
    val = op_sadd(list16->i9, inc);
    op_list16_in_i9(list16, val);
    break;
  case 11:
    val = op_sadd(list16->i10, inc);
    op_list16_in_i10(list16, val);
    break;
  case 12:
    val = op_sadd(list16->i11, inc);
    op_list16_in_i11(list16, val);
    break;
  case 13:
    val = op_sadd(list16->i12, inc);
    op_list16_in_i12(list16, val);
    break;
  case 14:
    val = op_sadd(list16->i13, inc);
    op_list16_in_i13(list16, val);
    break;
  case 15:
    val = op_sadd(list16->i14, inc);
    op_list16_in_i14(list16, val);
    break;
  case 16:
    val = op_sadd(list16->i15, inc);
    op_list16_in_i15(list16, val);
    break;         
  }
}
*/

// pickle / unpickle
u8* op_list16_pickle(op_list16_t* op, u8* dst) {
  dst = pickle_io(op->index, dst);
  dst = pickle_io(op->i0, dst);
  dst = pickle_io(op->i1, dst);
  dst = pickle_io(op->i2, dst);
  dst = pickle_io(op->i3, dst);
  dst = pickle_io(op->i4, dst);
  dst = pickle_io(op->i5, dst);
  dst = pickle_io(op->i6, dst);
  dst = pickle_io(op->i7, dst);
  dst = pickle_io(op->i8, dst);
  dst = pickle_io(op->i9, dst);
  dst = pickle_io(op->i10, dst);
  dst = pickle_io(op->i11, dst);
  dst = pickle_io(op->i12, dst);
  dst = pickle_io(op->i13, dst);
  dst = pickle_io(op->i14, dst);
  dst = pickle_io(op->i15, dst);  

  return dst;
}

const u8* op_list16_unpickle(op_list16_t* op, const u8* src) {
  src = unpickle_io(src, &(op->index));
  src = unpickle_io(src, &(op->i0));
  src = unpickle_io(src, &(op->i1));
  src = unpickle_io(src, &(op->i2));
  src = unpickle_io(src, &(op->i3));
  src = unpickle_io(src, &(op->i4));
  src = unpickle_io(src, &(op->i5));
  src = unpickle_io(src, &(op->i6));
  src = unpickle_io(src, &(op->i7));
  src = unpickle_io(src, &(op->i8));
  src = unpickle_io(src, &(op->i9));
  src = unpickle_io(src, &(op->i10));
  src = unpickle_io(src, &(op->i11));
  src = unpickle_io(src, &(op->i12));
  src = unpickle_io(src, &(op->i13));
  src = unpickle_io(src, &(op->i14));
  src = unpickle_io(src, &(op->i15));

  return src;
}
