#include "net_protected.h"
#include "op_div.h"

//-------------------------------------------------
//----- static function declaration
static void op_div_inc_input(op_div_t* div, const s16 idx, const io_t inc);
static void op_div_in_a(op_div_t* div, const io_t v);
static void op_div_in_b(op_div_t* div, const io_t v);
static void op_div_in_btrig(op_div_t* div, const io_t v);

// pickle / unpickle
static u8* op_div_pickle(op_div_t* op, u8* dst);
static const u8* op_div_unpickle(op_div_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_div_in_fn[3] = {
  (op_in_fn)&op_div_in_a,
  (op_in_fn)&op_div_in_b,
  (op_in_fn)&op_div_in_btrig
};

static const char* op_div_instring  = "A       B       B_TRIG  ";
static const char* op_div_outstring = "VAL     ";
static const char* op_div_opstring  = "DIV";

//-------------------------------------------------
//----- external function definitions
void op_div_init(void* mem) {
  op_div_t* div = (op_div_t*)mem;
  div->super.numInputs = 3;
  div->super.numOutputs = 1;
  div->outs[0] = -1;

  div->super.inc_fn = (op_inc_fn)op_div_inc_input;
  div->super.in_fn = op_div_in_fn;
  div->super.pickle = (op_pickle_fn) (&op_div_pickle);
  div->super.unpickle = (op_unpickle_fn) (&op_div_unpickle);

  div->super.in_val = div->in_val;
  div->super.out = div->outs;
  div->super.opString = op_div_opstring;
  div->super.inString = op_div_instring;
  div->super.outString = op_div_outstring;
  div->super.type = eOpDiv;  
  div->in_val[0] = &(div->a);
  div->in_val[1] = &(div->b);
  div->in_val[2] = &(div->btrig);

  div->a = 1;
  div->b = 1;
  div->btrig = 0;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_div_in_a(op_div_t* div, const io_t v) {
  // printf("div at %d received A %d\n", (int)div, (int)*v);
  div->a = v;
  div->val = op_div(div->a, div->b);
  net_activate(div->outs[0], div->val, div);
}

// set operand B
static void op_div_in_b(op_div_t* div, const io_t v) {
  //printf("div at %d received B %d\n", (int)div, (int)*v);
  if(v == 0) { div->b = 1; } else {
    div->b = v;
  }
  div->val = op_div(div->a, div->b);
  if(div->btrig) {
    net_activate(div->outs[0], div->val, div);
  }
}

// set b-trigger mode
static void op_div_in_btrig(op_div_t* div, const io_t v) {
  //printf("div at %d received BTRIG %d\n", (int)div, (int)*v);
  if(v > 0) { div->btrig = OP_ONE; } else { div->btrig = 0; }
}

//===== UI input
static void op_div_inc_input(op_div_t* div, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0:  // a
    val = op_add(div->a, inc);
    op_div_in_a(div, val);
    break; 
  case 1:  // b
    val = op_add(div->b, inc);
    op_div_in_b(div, val);
    break;
  case 2:  // trig
    op_div_in_btrig(div, inc);
    break;
  }
}

// pickle / unpickle
u8* op_div_pickle(op_div_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_div_unpickle(op_div_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
