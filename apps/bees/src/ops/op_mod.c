#include "net_protected.h"
#include "op_mod.h"

//-------------------------------------------------
//----- static function declaration
static void op_mod_in_a(op_mod_t* mod, const io_t v);
static void op_mod_in_b(op_mod_t* mod, const io_t v);
static void op_mod_in_btrig(op_mod_t* mod, const io_t v);

// pickle / unpickle
static u8* op_mod_pickle(op_mod_t* op, u8* dst);
static const u8* op_mod_unpickle(op_mod_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_mod_in_fn[3] = {
  (op_in_fn)&op_mod_in_a,
  (op_in_fn)&op_mod_in_b,
  (op_in_fn)&op_mod_in_btrig
};

static const char* op_mod_instring  = "A\0      B\0      TRIG\0   ";
static const char* op_mod_outstring = "VAL\0    ";
static const char* op_mod_opstring  = "MOD";

//-------------------------------------------------
//----- external function definitions
void op_mod_init(void* mem) {
  op_mod_t* mod = (op_mod_t*)mem;
  mod->super.numInputs = 3;
  mod->super.numOutputs = 1;
  mod->outs[0] = -1;

  mod->super.in_fn = op_mod_in_fn;
  mod->super.pickle = (op_pickle_fn) (&op_mod_pickle);
  mod->super.unpickle = (op_unpickle_fn) (&op_mod_unpickle);

  mod->super.in_val = mod->in_val;
  mod->super.out = mod->outs;
  mod->super.opString = op_mod_opstring;
  mod->super.inString = op_mod_instring;
  mod->super.outString = op_mod_outstring;
  mod->super.type = eOpMod;  
  mod->in_val[0] = &(mod->a);
  mod->in_val[1] = &(mod->b);
  mod->in_val[2] = &(mod->btrig);

  mod->a = 1;
  mod->b = 1;
  mod->btrig = 0;
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_mod_in_a(op_mod_t* mod, const io_t v) {
  // printf("mod at %d received A %d\n", (int)mod, (int)*v);
  mod->a = v;
  mod->val = (mod->a % mod->b);
  net_activate(mod->outs[0], mod->val, mod);
}

// set operand B
static void op_mod_in_b(op_mod_t* mod, const io_t v) {
  //printf("mod at %d received B %d\n", (int)mod, (int)*v);
  if(v == 0) { mod->b = 1; } else {
    mod->b = v;
  }
  mod->val = (mod->a % mod->b);
  if(mod->btrig) {
    net_activate(mod->outs[0], mod->val, mod);
  }
}

// set b-trigger mode
static void op_mod_in_btrig(op_mod_t* mod, const io_t v) {
  //printf("mod at %d received BTRIG %d\n", (int)mod, (int)*v);
  if(v > 0) { mod->btrig = OP_ONE; } else { mod->btrig = 0; }
}

// pickle / unpickle
u8* op_mod_pickle(op_mod_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->btrig, dst);
  return dst;
}

const u8* op_mod_unpickle(op_mod_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->btrig));
  return src;
}
