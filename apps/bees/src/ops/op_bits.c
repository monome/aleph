#include "net_protected.h"
#include "op_bits.h"

//-------------------------------------------------
//---- static func declare
static void op_bits_in_index(op_bits_t* bits, const io_t v);
static void op_bits_in_i0(op_bits_t* bits, const io_t v);
static void op_bits_in_i1(op_bits_t* bits, const io_t v);
static void op_bits_in_i2(op_bits_t* bits, const io_t v);
static void op_bits_in_i3(op_bits_t* bits, const io_t v);
static void op_bits_in_i4(op_bits_t* bits, const io_t v);
static void op_bits_in_i5(op_bits_t* bits, const io_t v);
static void op_bits_in_i6(op_bits_t* bits, const io_t v);
static void op_bits_in_i7(op_bits_t* bits, const io_t v);

// output on input from inputs
static void op_bits_output(op_bits_t* bits);

// pickle / unpickle
static u8* op_bits_pickle(op_bits_t* op, u8* dst);
static const u8* op_bits_unpickle(op_bits_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_bits_instring = "IN\0     I0\0     I1\0     I2\0     I3\0     I4\0     I5\0     I6\0     I7\0     ";
static const char* op_bits_outstring = "OUT\0    O0\0     O1\0     O2\0     O3\0     O4\0     O5\0     O6\0     O7\0     ";
static const char* op_bits_opstring = "BITS";

static op_in_fn op_bits_in_fn[9] = {
  (op_in_fn)&op_bits_in_index,
  (op_in_fn)&op_bits_in_i0,
  (op_in_fn)&op_bits_in_i1,
  (op_in_fn)&op_bits_in_i2,
  (op_in_fn)&op_bits_in_i3,
  (op_in_fn)&op_bits_in_i4,
  (op_in_fn)&op_bits_in_i5,
  (op_in_fn)&op_bits_in_i6,
  (op_in_fn)&op_bits_in_i7
};

//-------------------------------------------------
//---- external func define
void op_bits_init(void* mem) {
  op_bits_t* bits = (op_bits_t*)mem;
  bits->super.numInputs = 9;
  bits->super.numOutputs = 9;
  bits->outs[0] = -1;
  bits->outs[1] = -1;
  bits->outs[2] = -1;
  bits->outs[3] = -1;
  bits->outs[4] = -1;
  bits->outs[5] = -1;
  bits->outs[6] = -1;
  bits->outs[7] = -1;
  bits->outs[8] = -1;

  bits->super.in_fn = op_bits_in_fn;
  bits->super.in_val = bits->in_val;
  bits->super.pickle = (op_pickle_fn) (&op_bits_pickle);
  bits->super.unpickle = (op_unpickle_fn) (&op_bits_unpickle);

  bits->super.out = bits->outs;
  bits->super.opString = op_bits_opstring;
  bits->super.inString = op_bits_instring;
  bits->super.outString = op_bits_outstring;
  bits->super.type = eOpBits;  

  bits->in_val[0] = &(bits->index);
  bits->in_val[1] = &(bits->i0);
  bits->in_val[2] = &(bits->i1);
  bits->in_val[3] = &(bits->i2);
  bits->in_val[4] = &(bits->i3);
  bits->in_val[5] = &(bits->i4);
  bits->in_val[6] = &(bits->i5);
  bits->in_val[7] = &(bits->i6);
  bits->in_val[8] = &(bits->i7);

  bits->index = 0;
  bits->i0 = 0;
  bits->i1 = 0;
  bits->i2 = 0;
  bits->i3 = 0;
  bits->i4 = 0;
  bits->i5 = 0;
  bits->i6 = 0;
  bits->i7 = 0;
}

//-------------------------------------------------
//---- static func define

static void op_bits_in_index(op_bits_t* bits, const io_t val) {
  bits->index = val;
  if(bits->index<0) bits->index = 0;
  if(bits->index>255) bits->index = 255;
  

  for(u8 i=0;i<8;i++) {
      net_activate(bits->outs[i+1], !!(bits->index & (1<<i)), bits);    // !! turns num into 0 or 1
  }
}

static void op_bits_in_i0(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i0 = OP_ONE; } else { bits->i0 = 0; }
  op_bits_output(bits);
}

static void op_bits_in_i1(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i1 = OP_ONE; } else { bits->i1 = 0; }
  op_bits_output(bits);
}

static void op_bits_in_i2(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i2 = OP_ONE; } else { bits->i2 = 0; }
  op_bits_output(bits);
}

static void op_bits_in_i3(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i3 = OP_ONE; } else { bits->i3 = 0; }
  op_bits_output(bits);
}

static void op_bits_in_i4(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i4 = OP_ONE; } else { bits->i4 = 0; }
  op_bits_output(bits);
}

static void op_bits_in_i5(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i5 = OP_ONE; } else { bits->i5 = 0; }
  op_bits_output(bits);
}

static void op_bits_in_i6(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i6 = OP_ONE; } else { bits->i6 = 0; }
  op_bits_output(bits);
}

static void op_bits_in_i7(op_bits_t* bits, const io_t val) {
  if(val > 0) { bits->i7 = OP_ONE; } else { bits->i7 = 0; }
  op_bits_output(bits);
}

// output after input from individual bits
static void op_bits_output(op_bits_t* bits) {
  u8 val = 0;

  val += bits->i0 << 0;
  val += bits->i1 << 1;
  val += bits->i2 << 2;
  val += bits->i3 << 3;
  val += bits->i4 << 4;
  val += bits->i5 << 5;
  val += bits->i6 << 6;
  val += bits->i7 << 7;

  net_activate(bits->outs[0], val, bits);
}



// pickle / unpickle
u8* op_bits_pickle(op_bits_t* op, u8* dst) {
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

const u8* op_bits_unpickle(op_bits_t* op, const u8* src) {
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
