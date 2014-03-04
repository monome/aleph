#include "net_protected.h"
#include "op_logic.h"

//-------------------------------------------------
//----- static function declaration
static void op_logic_in_a(op_logic_t* logic, const io_t v);
static void op_logic_in_b(op_logic_t* logic, const io_t v);
static void op_logic_in_btrig(op_logic_t* logic, const io_t v);
static void op_logic_in_edge(op_logic_t* logic, const io_t v);
static void op_logic_in_invert(op_logic_t* logic, const io_t v);

// pickle / unpickle
static u8* op_logic_pickle(op_logic_t* op, u8* dst);
static const u8* op_logic_unpickle(op_logic_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_logic_in_fn[5] = {
  (op_in_fn)&op_logic_in_a,
  (op_in_fn)&op_logic_in_b,
  (op_in_fn)&op_logic_in_btrig,
  (op_in_fn)&op_logic_in_edge,
  (op_in_fn)&op_logic_in_invert
};

static const char* op_logic_instring  = "A\0      B\0      TRIG\0   EDGE\0   INVERT\0 ";
static const char* op_logic_outstring = "AND\0    OR\0     XOR\0    ";
static const char* op_logic_opstring  = "LOGIC";

//-------------------------------------------------
//----- external function definitions
void op_logic_init(void* mem) {
  op_logic_t* logic = (op_logic_t*)mem;
  logic->super.numInputs = 5;
  logic->super.numOutputs = 3;
  logic->outs[0] = -1;
  logic->outs[1] = -1;
  logic->outs[2] = -1;

  logic->super.in_fn = op_logic_in_fn;
  logic->super.pickle = (op_pickle_fn) (&op_logic_pickle);
  logic->super.unpickle = (op_unpickle_fn) (&op_logic_unpickle);

  logic->super.in_val = logic->in_val;
  logic->super.out = logic->outs;
  logic->super.opString = op_logic_opstring;
  logic->super.inString = op_logic_instring;
  logic->super.outString = op_logic_outstring;
  logic->super.type = eOpLogic;  
  logic->in_val[0] = &(logic->a);
  logic->in_val[1] = &(logic->b);
  logic->in_val[2] = &(logic->btrig);
  logic->in_val[3] = &(logic->edge);
  logic->in_val[4] = &(logic->invert);

  logic->a = 0;
  logic->b = 1;
  logic->btrig = 1;
  logic->edge = 0;
  logic->invert = 0;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_logic_in_a(op_logic_t* logic, const io_t v) {
  // printf("logic at %d received A %d\n", (int)logic, (int)*v);
  u8 i;

  if(v > 0) { logic->a = OP_ONE; } else { logic->a = 0; }

  if(logic->edge) {
    i = logic->v_and;
    logic->v_and = (logic->a && logic->b) ^ logic->invert;
    if(i != logic->v_and)
      net_activate(logic->outs[0], logic->v_and, logic);

    i = logic->v_or;
    logic->v_or = (logic->a || logic->b) ^ logic->invert;
    if(i != logic->v_or)
      net_activate(logic->outs[1], logic->v_or, logic);

    i = logic->v_xor;
    logic->v_xor = (logic->a ^ logic->b) ^ logic->invert;
    if(i != logic->v_xor)
      net_activate(logic->outs[2], logic->v_xor, logic);
  } 
  else {
    logic->v_and = (logic->a && logic->b) ^ logic->invert;
    net_activate(logic->outs[0], logic->v_and, logic);

    logic->v_or = (logic->a || logic->b) ^ logic->invert;
    net_activate(logic->outs[1], logic->v_or, logic);

    logic->v_xor = (logic->a ^ logic->b) ^ logic->invert;
    net_activate(logic->outs[2], logic->v_xor, logic);
  }
}

// set operand B
static void op_logic_in_b(op_logic_t* logic, const io_t v) {
  //printf("logic at %d received B %d\n", (int)logic, (int)*v);
  if(v > 0) { logic->b = OP_ONE; } else { logic->b = 0; }
  
  if(logic->btrig) {
    op_logic_in_a(logic, logic->a);   // TRIGGER
  }
}

// set b-trigger mode
static void op_logic_in_btrig(op_logic_t* logic, const io_t v) {
  //printf("logic at %d received BTRIG %d\n", (int)logic, (int)*v);
  if(v > 0) { logic->btrig = OP_ONE; } else { logic->btrig = 0; }
}

// set edge mode (if 1, output on change only)
static void op_logic_in_edge(op_logic_t* logic, const io_t v) {
  //printf("logic at %d received BTRIG %d\n", (int)logic, (int)*v);
  if(v > 0) { 
    logic->edge = OP_ONE; 
    op_logic_in_a(logic, logic->a);   // TRIGGER ON ENABLE
  } else { logic->edge = 0; }
}

// set bit inversion
static void op_logic_in_invert(op_logic_t* logic, const io_t v) {
  //printf("logic at %d received BTRIG %d\n", (int)logic, (int)*v);
  if(v > 0) { 
    logic->invert = OP_ONE; 
    op_logic_in_a(logic, logic->a);   // TRIGGER ON ENABLE
  } else { logic->invert = 0; }
}


// pickle / unpickle
u8* op_logic_pickle(op_logic_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  dst = pickle_io(op->edge, dst);
  dst = pickle_io(op->invert, dst);
  return dst;
}

const u8* op_logic_unpickle(op_logic_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  src = unpickle_io(src, &(op->edge));
  src = unpickle_io(src, &(op->invert));
  return src;
}
