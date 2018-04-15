#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_route.h"

//-------------------------------------------------
//----- descriptor
static const char* op_route2_instring =  "VAL\0    TO\0     ";
static const char* op_route2_outstring = "O0\0     O1\0     ";
static const char* op_route2_opstring = "ROUTE2";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_route2_in_val(op_route2_t* route, const io_t v);
static void op_route2_in_to(op_route2_t* route, const io_t v);
// pickle / unpickle
static u8* op_route2_pickle(op_route2_t* route, u8* dst);
const static u8* op_route2_unpickle(op_route2_t* route, const u8* src);

// array of input functions 
static op_in_fn op_route2_in[2] = {
  (op_in_fn)&op_route2_in_val,
  (op_in_fn)&op_route2_in_to
};

//----- external function definition

/// initialize
void op_route2_init(void* op) {
  op_route2_t* route = (op_route2_t*)op;

  // superclass functions
  route->super.in_fn = op_route2_in;
  route->super.pickle = (op_pickle_fn) (&op_route2_pickle);
  route->super.unpickle = (op_unpickle_fn) (&op_route2_unpickle);
  
  // superclass state
  route->super.numInputs = 2;
  route->super.numOutputs = 2;
  route->outs[0] = -1;
  route->outs[1] = -1;
 
  route->super.in_val = route->in_val;
  route->in_val[0] = &(route->val);
  route->in_val[1] = &(route->to);

  route->super.out = route->outs;
  route->super.opString = op_route2_opstring;
  route->super.inString = op_route2_instring;
  route->super.outString = op_route2_outstring;
  route->super.type = eOpRoute2;

  // class state
  route->val = 0;
  route->to = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_route2_in_val(op_route2_t* route, const io_t v) {
  route->val = v;
  net_activate(route, route->to, route->val);
}

// to where
static void op_route2_in_to(op_route2_t* route, const io_t v) {
  io_t val = v;
  if(val<0) val = 0;
  else if(val>1) val = 1;
  route->to = val;
}


// serialization
u8* op_route2_pickle(op_route2_t* route, u8* dst) {
  dst = pickle_io(route->to, dst);
  return dst;
}

const u8* op_route2_unpickle(op_route2_t* route, const u8* src) {
  src = unpickle_io(src, &(route->to));
  return src;
}
