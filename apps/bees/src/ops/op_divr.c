#include "net_protected.h"
#include "op_divr.h"

//-------------------------------------------------
//----- static function declaration
static void op_divr_in_a(op_divr_t* divr, const io_t v);
static void op_divr_in_b(op_divr_t* divr, const io_t v);
static void op_divr_in_btrig(op_divr_t* divr, const io_t v);

// pickle / unpickle
static u8* op_divr_pickle(op_divr_t* op, u8* dst);
static const u8* op_divr_unpickle(op_divr_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_divr_in_fn[3] = {
  (op_in_fn)&op_divr_in_a,
  (op_in_fn)&op_divr_in_b,
  (op_in_fn)&op_divr_in_btrig
};

static const char* op_divr_instring  = "A\0      B\0      TRIG\0   ";
static const char* op_divr_outstring = "VAL\0    R\0      ";
static const char* op_divr_opstring  = "DIVr";

//-------------------------------------------------
//----- external function definitions
void op_divr_init(void* mem) {
  op_divr_t* divr = (op_divr_t*)mem;
  divr->super.numInputs = 3;
  divr->super.numOutputs = 2;
  divr->outs[0] = -1;
  divr->outs[1] = -1;

  divr->super.in_fn = op_divr_in_fn;
  divr->super.pickle = (op_pickle_fn) (&op_divr_pickle);
  divr->super.unpickle = (op_unpickle_fn) (&op_divr_unpickle);

  divr->super.in_val = divr->in_val;
  divr->super.out = divr->outs;
  divr->super.opString = op_divr_opstring;
  divr->super.inString = op_divr_instring;
  divr->super.outString = op_divr_outstring;
  divr->super.type = eOpDivr;  
  divr->in_val[0] = &(divr->a);
  divr->in_val[1] = &(divr->b);
  divr->in_val[2] = &(divr->btrig);

  divr->a = 1;
  divr->b = 1;
  divr->btrig = 0;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_divr_in_a(op_divr_t* divr, const io_t v) {
  // printf("divr at %d received A %d\n", (int)divr, (int)*v);
  divr->a = v;
  divr->val = op_div(divr->a, divr->b);
  divr->r = divr->a % divr->b;
  net_activate(divr->outs[0], divr->val, divr);
  net_activate(divr->outs[1], divr->r, divr);
}

// set operand B
static void op_divr_in_b(op_divr_t* divr, const io_t v) {
  //printf("divr at %d received B %d\n", (int)divr, (int)*v);
  if(v == 0) { divr->b = 1; } else {
    divr->b = v;
  }
  divr->val = op_div(divr->a, divr->b);
  divr->r = divr->a % divr->b;
  if(divr->btrig) {
    net_activate(divr->outs[0], divr->val, divr);
    net_activate(divr->outs[1], divr->r, divr);
  }
}

// set b-trigger mode
static void op_divr_in_btrig(op_divr_t* divr, const io_t v) {
  //printf("divr at %d received BTRIG %d\n", (int)divr, (int)*v);
  if(v > 0) { divr->btrig = OP_ONE; } else { divr->btrig = 0; }
}


// pickle / unpickle
u8* op_divr_pickle(op_divr_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_divr_unpickle(op_divr_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
