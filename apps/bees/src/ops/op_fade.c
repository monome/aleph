#include "net_protected.h"
#include "op_fade.h"

//-------------------------------------------------
//---- static func declare
// functions for inputs
static void op_fade_in_a(op_fade_t* fade, const io_t v);
static void op_fade_in_b(op_fade_t* fade, const io_t v);
static void op_fade_in_x(op_fade_t* fade, const io_t v);

// pickle / unpickle
// for scene saving
static u8* op_fade_pickle(op_fade_t* op, u8* dst);
static const u8* op_fade_unpickle(op_fade_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
// in/out strings must be 8 chars long. terminate with NULL \0
static const char* op_fade_instring = "A\0      B\0      X\0      ";
static const char* op_fade_outstring = "VAL\0    ";
static const char* op_fade_opstring = "FADE";

// assign input function table
static op_in_fn op_fade_in_fn[3] = {
  (op_in_fn)&op_fade_in_a,
  (op_in_fn)&op_fade_in_b, 
  (op_in_fn)&op_fade_in_x
};

//-------------------------------------------------
//---- external func define
// intialization
void op_fade_init(void* mem) {
  op_fade_t* fade = (op_fade_t*)mem;
  fade->super.numInputs = 3;
  fade->super.numOutputs = 1;
  fade->outs[0] = -1;

  fade->super.in_fn = op_fade_in_fn;
  fade->super.in_val = fade->in_val;
  fade->super.pickle = (op_pickle_fn) (&op_fade_pickle);
  fade->super.unpickle = (op_unpickle_fn) (&op_fade_unpickle);

  fade->super.out = fade->outs;
  fade->super.opString = op_fade_opstring;
  fade->super.inString = op_fade_instring;
  fade->super.outString = op_fade_outstring;
  // IMPORTANT: enum below
  fade->super.type = eOpFade;  

  fade->in_val[0] = &(fade->a);
  fade->in_val[1] = &(fade->b);
  fade->in_val[2] = &(fade->x);

  fade->a = 0;
  fade->b = 0;
  fade->x = 0;
}

//-------------------------------------------------
//---- static func define

static void op_fade_in_a(op_fade_t* fade, const io_t v) {
  fade->a = v;
  // do the math for crossfade:
  fade->val = (fade->a * (128 - fade->x)) / 128 + (fade->b * fade->x) / 128;
  // activate output
  net_activate(fade->outs[0], fade->val, fade);
}

static void op_fade_in_b(op_fade_t* fade, const io_t v) {
  fade->b = v;
  fade->val = (fade->a * (128 - fade->x)) / 128 + (fade->b * fade->x) / 128;
  net_activate(fade->outs[0], fade->val, fade);
}

static void op_fade_in_x(op_fade_t* fade, const io_t v) {
  fade->x = v;
  // limit range of x
  if(fade->x < 0) fade->x = 0;
  else if(fade->x > 128) fade->x = 128;

  fade->val = (fade->a * (128 - fade->x)) / 128 + (fade->b * fade->x) / 128;
  net_activate(fade->outs[0], fade->val, fade);
}


// pickle / unpickle
// for scene saving
u8* op_fade_pickle(op_fade_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  dst = pickle_io(op->x, dst);
  return dst;
}

const u8* op_fade_unpickle(op_fade_t* op, const u8* src) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  src = unpickle_io(src, &(op->x));
  return src;
}
