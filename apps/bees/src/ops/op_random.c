#include "net_protected.h"
#include "op_random.h"

//-------------------------------------------------
//---- static func declare
static void countBits(op_random_t* random);
static void op_random_in_min(op_random_t* min, const io_t v);
static void op_random_in_max(op_random_t* max, const io_t v);
static void op_random_in_trig(op_random_t* trig, const io_t v);
static void op_random_in_seed(op_random_t* seed, const io_t v);

// pickle / unpickle
static u8* op_random_pickle(op_random_t* op, u8* dst);
static const u8* op_random_unpickle(op_random_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_random_instring = "MIN\0    MAX\0    TRIG\0   SEED\0   ";
static const char* op_random_outstring = "VAL     ";
static const char* op_random_opstring = "RAND";

static op_in_fn op_random_in_fn[4] = {
  (op_in_fn)&op_random_in_min,
  (op_in_fn)&op_random_in_max, 
  (op_in_fn)&op_random_in_trig,
  (op_in_fn)&op_random_in_seed
};

//-------------------------------------------------
//---- external func define
void op_random_init(void* mem) {
  op_random_t* random = (op_random_t*)mem;
  random->super.numInputs = 4;
  random->super.numOutputs = 1;
  random->outs[0] = -1;

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
  random->in_val[3] = &(random->seed);

  random->min = 0;
  random->max = 1;
  random->trig = 0;
  random->seed = 111;

  random->a = 0x19660d;
  random->c = 0x3c6ef35f;
  random->x = 111;  // FIXME seed... do this better
  countBits(random);
}

//-------------------------------------------------
//---- static func define

static void countBits(op_random_t* random) {
  random->range = random->max - random->min;
  u16 n = random->range;
  int counter = 0;
  while(n) {
    counter ++;
    n = n >> 1;
  }
  random->bitShift = 25 - counter;
}

static void op_random_in_min(op_random_t* random, const io_t v) {
  if(v <= random->max) {
    random->min = v;
  }
  else {
    random->min = random->max;
  }
  countBits(random);
}

static void op_random_in_max(op_random_t* random, const io_t v) {
  if(v >= random->min) {
    random->max = v;
  }
  else {
    random->max = random->min;
  }
  countBits(random);
}

static void op_random_in_seed(op_random_t* random, const io_t v) {
  random->seed = v;
  random->x = v;
}

static void op_random_in_trig(op_random_t* random, const io_t v) {
  random->x = random->x * random->c + random->a;
  random->val = (random->x >> random->bitShift);
  random->val %= random->range;
  random->val += random->min;
  /* printf("%d\n", random->val); */
  net_activate(random, 0, random->val);
}


// pickle / unpickle
u8* op_random_pickle(op_random_t* op, u8* dst) {
  dst = pickle_io(op->min, dst);
  dst = pickle_io(op->max, dst);
  dst = pickle_io(op->trig, dst);
  dst = pickle_io(op->seed, dst);
  return dst;
}

const u8* op_random_unpickle(op_random_t* op, const u8* src) {
  src = unpickle_io(src, &(op->min));
  src = unpickle_io(src, &(op->max));
  src = unpickle_io(src, &(op->trig));
  src = unpickle_io(src, &(op->seed));
  return src;
}

