#include "net_protected.h"
#include "op_is.h"

//-------------------------------------------------
//----- static function declaration
static void op_is_in_a(op_is_t* is, const io_t v);
static void op_is_in_b(op_is_t* is, const io_t v);
static void op_is_in_btrig(op_is_t* is, const io_t v);
static void op_is_in_edge(op_is_t* is, const io_t v);

// pickle / unpickle
static u8* op_is_pickle(op_is_t* op, u8* dst);
static const u8* op_is_unpickle(op_is_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_is_in_fn[4] = {
  (op_in_fn)&op_is_in_a,
  (op_in_fn)&op_is_in_b,
  (op_in_fn)&op_is_in_btrig,
  (op_in_fn)&op_is_in_edge
};

static const char* op_is_instring  = "A\0      B\0      TRIG\0   EDGE\0   ";
static const char* op_is_outstring = "EQ\0     NE\0     GT\0     LT\0     ";
static const char* op_is_opstring  = "IS";

//-------------------------------------------------
//----- external function definitions
void op_is_init(void* mem) {
  op_is_t* is = (op_is_t*)mem;
  is->super.numInputs = 4;
  is->super.numOutputs = 4;
  is->outs[0] = -1;
  is->outs[1] = -1;
  is->outs[2] = -1;
  is->outs[3] = -1;

  is->super.in_fn = op_is_in_fn;
  is->super.pickle = (op_pickle_fn) (&op_is_pickle);
  is->super.unpickle = (op_unpickle_fn) (&op_is_unpickle);

  is->super.in_val = is->in_val;
  is->super.out = is->outs;
  is->super.opString = op_is_opstring;
  is->super.inString = op_is_instring;
  is->super.outString = op_is_outstring;
  is->super.type = eOpIs;  
  is->in_val[0] = &(is->a);
  is->in_val[1] = &(is->b);
  is->in_val[2] = &(is->btrig);
  is->in_val[3] = &(is->edge);

  is->a = 0;
  is->b = 1;
  is->btrig = 0;
  is->edge = 1;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_is_in_a(op_is_t* is, const io_t v) {
  // printf("is at %d received A %d\n", (int)is, (int)*v);
  u8 i;

  is->a = v;

  if(is->edge) {
    i = is->eq;
    is->eq = (is->a == is->b);
    if(i != is->eq)
      net_activate(is->outs[0], is->eq, is);

    i = is->ne;
    is->ne = (is->a != is->b);
    if(i != is->ne)
      net_activate(is->outs[1], is->ne, is);

    i = is->gt;
    is->gt = (is->a > is->b);
    if(i != is->gt)
      net_activate(is->outs[2], is->gt, is);

    i = is->lt;
    is->lt = (is->a < is->b);
    if(i != is->lt)
      net_activate(is->outs[3], is->lt, is);
  } 
  else {
    is->eq = (is->a == is->b);
    net_activate(is->outs[0], is->eq, is);

    is->ne = (is->a != is->b);
    net_activate(is->outs[1], is->ne, is);

    is->gt = (is->a > is->b);
    net_activate(is->outs[2], is->gt, is);

    is->lt = (is->a < is->b);
    net_activate(is->outs[3], is->lt, is);
  }
}

// set operand B
static void op_is_in_b(op_is_t* is, const io_t v) {
  //printf("is at %d received B %d\n", (int)is, (int)*v);
  is->b = v;
  
  if(is->btrig) {
     // is this ok?
    op_is_in_a(is, is->a);
  }
}

// set b-trigger mode
static void op_is_in_btrig(op_is_t* is, const io_t v) {
  //printf("is at %d received BTRIG %d\n", (int)is, (int)*v);
  if(v > 0) { is->btrig = OP_ONE; } else { is->btrig = 0; }
}

// set edge mode (if 1, output on change only)
static void op_is_in_edge(op_is_t* is, const io_t v) {
  //printf("is at %d received BTRIG %d\n", (int)is, (int)*v);
  if(v > 0) { is->edge = OP_ONE; } else { is->edge = 0; }
}


// pickle / unpickle
u8* op_is_pickle(op_is_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  dst = pickle_io(op->edge, dst);
  return dst;
}

const u8* op_is_unpickle(op_is_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  src = unpickle_io(src, &(op->edge));
  return src;
}
