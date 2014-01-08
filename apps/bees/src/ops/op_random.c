#include "net_protected.h"
#include "op_random.h"

//-------------------------------------------------
//---- static func declare
static void op_random_in_min(op_random_t* min, const io_t v);
static void op_random_in_max(op_random_t* max, const io_t v);
static void op_random_in_trig(op_random_t* trig, const io_t v);
static void op_random_inc_input(op_random_t* mul, const s16 idx, const io_t inc);

// pickle / unpickle
static u8* op_random_pickle(op_random_t* op, u8* dst);
static const u8* op_random_unpickle(op_random_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_random_instring = "MIN     MAX     TRIG    ";
static const char* op_random_outstring = "VAL     ";
static const char* op_random_opstring = "RANDOM";

static op_in_fn op_random_in_fn[3] = {
  (op_in_fn)&op_random_in_min,
  (op_in_fn)&op_random_in_max, 
  (op_in_fn)&op_random_in_trig
};

//-------------------------------------------------
//---- external func define
void op_random_init(void* mem) {
  op_random_t* random = (op_random_t*)mem;
  random->super.numInputs = 3;
  random->super.numOutputs = 1;
  random->outs[0] = -1;

  random->super.inc_fn = (op_inc_fn)op_random_inc_input;
  random->super.in_fn = op_random_in_fn;
  random->super.in_val = random->in_val;
  random->super.pickle = (op_pickle_fn) (&op_random_pickle);
  random->super.unpickle = (op_unpickle_fn) (&op_random_unpickle);

  random->super.out = random->outs;
  random->super.opString = op_random_opstring;
  random->super.inString = op_random_instring;
  random->super.outString = op_random_outstring;
  random->super.type = eOpRandom;  

  random->in_val[0] = &(random->min);
  random->in_val[1] = &(random->max);
  random->in_val[2] = &(random->trig);

  random->min = 0;
  random->max = 1;
  random->trig = 0;

  random->a = 0x19660d;
  random->c = 0x3c6ef35f;
  random->x = 111;  // FIXME seed... do this better
}

//-------------------------------------------------
//---- static func define

static void op_random_in_min(op_random_t* random, const io_t v) {
  random->min = v;
}

static void op_random_in_max(op_random_t* random, const io_t v) {
  random->max = v;
}

static void op_random_in_trig(op_random_t* random, const io_t v) {
  random->x = random->x * random->c + random->a;
  random->val =random->x;
  if(random->val < 0)
    random->val *= -1;
  random->val = (random->val % ((random->max - random->min) + 1)) + random->min;
  net_activate(random->outs[0], random->val, random);
  // if(v > 0) { random->trig = OP_ONE; } else { random->trig = 0; }
}

//===== UI input
static void op_random_inc_input(op_random_t* random, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0:  // a
    val = op_sadd(random->min, inc);
    op_random_in_min(random, val);
    break; 
  case 1:  // b
    val = op_sadd(random->max, inc);
    op_random_in_max(random, val);
    break;
  case 2:  // trig
    op_random_in_trig(random, inc);
    break;
  }
}


// pickle / unpickle
u8* op_random_pickle(op_random_t* op, u8* dst) {
  dst = pickle_io(op->min, dst);
  dst = pickle_io(op->max, dst);
  dst = pickle_io(op->trig, dst);
  return dst;
}

const u8* op_random_unpickle(op_random_t* op, const u8* src) {
  src = unpickle_io(src, &(op->min));
  src = unpickle_io(src, &(op->max));
  src = unpickle_io(src, &(op->trig));
  return src;
}

