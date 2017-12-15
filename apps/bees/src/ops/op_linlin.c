#include "net_protected.h"
#include "op_linlin.h"

//-------------------------------------------------
//----- static function declaration
static void op_linlin_in_in(op_linlin_t* linlin, const io_t v);
static void op_linlin_in_imin(op_linlin_t* linlin, const io_t v);
static void op_linlin_in_imax(op_linlin_t* linlin, const io_t v);
static void op_linlin_in_omin(op_linlin_t* linlin, const io_t v);
static void op_linlin_in_omax(op_linlin_t* linlin, const io_t v);

// pickle / unpickle
static u8* op_linlin_pickle(op_linlin_t* op, u8* dst);
static const u8* op_linlin_unpickle(op_linlin_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_linlin_in_fn[5] = {
  (op_in_fn)&op_linlin_in_in,
  (op_in_fn)&op_linlin_in_imin,
  (op_in_fn)&op_linlin_in_imax,
  (op_in_fn)&op_linlin_in_omin,
  (op_in_fn)&op_linlin_in_omax,
};

static const char* op_linlin_instring  = "IN\0     IMIN\0   IMAX\0   OMIN\0   OMAX\0   ";
static const char* op_linlin_outstring = "VAL\0    ";
static const char* op_linlin_opstring  = "LINLIN";

//-------------------------------------------------
//----- external function definitions
void op_linlin_init(void* mem) {
  op_linlin_t* linlin = (op_linlin_t*)mem;
  linlin->super.numInputs = 5;
  linlin->super.numOutputs = 1;
  linlin->outs[0] = -1;

  linlin->super.in_fn = op_linlin_in_fn;
  linlin->super.pickle = (op_pickle_fn) (&op_linlin_pickle);
  linlin->super.unpickle = (op_unpickle_fn) (&op_linlin_unpickle);

  linlin->super.in_val = linlin->in_val;
  linlin->super.out = linlin->outs;
  linlin->super.opString = op_linlin_opstring;
  linlin->super.inString = op_linlin_instring;
  linlin->super.outString = op_linlin_outstring;
  linlin->super.type = eOpLinlin;

  linlin->in_val[0] = &(linlin->in);
  linlin->in_val[1] = &(linlin->imin);
  linlin->in_val[2] = &(linlin->imax);
  linlin->in_val[3] = &(linlin->omin);
  linlin->in_val[4] = &(linlin->omax);

  linlin->in = 0;
  linlin->imin = 0;
  linlin->imax = 1;
  linlin->omin = 0;
  linlin->omax = 1;
  linlin->a = 1;
  linlin->b = 0;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_linlin_in_in(op_linlin_t* linlin, const io_t v) {
  // out = (in - off) * a + b;
  linlin->in = v;
  s32 out = linlin->a * (v - linlin->off);
  out += linlin->b;
  out = out >> 16;
  net_activate(linlin, 0, out);
}
void calculate_a_and_b(op_linlin_t *linlin) {
  linlin->b = linlin->omin;
  linlin->off = linlin->imin;
  s32 mul = linlin->omax - linlin->omin;
  s32 div = linlin->imax - linlin->imin;
  linlin->a = mul << 16;
  if (div) {
    linlin->a /= div;
  }
}
static void op_linlin_in_imin(op_linlin_t* linlin, const io_t v) {
  linlin->imin = v;
  calculate_a_and_b(linlin);
}
static void op_linlin_in_imax(op_linlin_t* linlin, const io_t v) {
  linlin->imax = v;
  calculate_a_and_b(linlin);
}
static void op_linlin_in_omin(op_linlin_t* linlin, const io_t v) {
  linlin->omin = v;
  calculate_a_and_b(linlin);
}
static void op_linlin_in_omax(op_linlin_t* linlin, const io_t v) {
  linlin->omax = v;
  calculate_a_and_b(linlin);
}

// pickle / unpickle
u8* op_linlin_pickle(op_linlin_t* op, u8* dst) {
  dst = pickle_io(op->in, dst);
  dst = pickle_io(op->imin, dst);
  dst = pickle_io(op->imax, dst);
  dst = pickle_io(op->omin, dst);
  dst = pickle_io(op->omax, dst);
  return dst;
}

const u8* op_linlin_unpickle(op_linlin_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->in));
  src = unpickle_io(src, &(op->imin));
  src = unpickle_io(src, &(op->imax));
  src = unpickle_io(src, &(op->imin));
  src = unpickle_io(src, &(op->imax));
  return src;
}
