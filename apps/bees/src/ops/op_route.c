#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_route.h"

//-------------------------------------------------
//----- descriptor
static const char* op_route_instring =  "VAL\0    TO\0     ";
static const char* op_route_outstring = "O0\0     O1\0     O2\0     O3\0     ";
static const char* op_route_opstring = "ROUTE4";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_route_in_val(op_route_t* route, const io_t v);
static void op_route_in_to(op_route_t* route, const io_t v);
// pickle / unpickle
static u8* op_route_pickle(op_route_t* route, u8* dst);
const static u8* op_route_unpickle(op_route_t* route, const u8* src);

// array of input functions 
static op_in_fn op_route_in[2] = {
  (op_in_fn)&op_route_in_val,
  (op_in_fn)&op_route_in_to
};

//----- external function definition

/// initialize
void op_route_init(void* op) {
  op_route_t* route = (op_route_t*)op;

  // superclass functions
  route->super.in_fn = op_route_in;
  route->super.pickle = (op_pickle_fn) (&op_route_pickle);
  route->super.unpickle = (op_unpickle_fn) (&op_route_unpickle);
  
  // superclass state
  route->super.numInputs = 2;
  route->super.numOutputs = 4;
  route->outs[0] = -1;
  route->outs[1] = -1;
  route->outs[2] = -1;
  route->outs[3] = -1;
 
  route->super.in_val = route->in_val;
  route->in_val[0] = &(route->val);
  route->in_val[1] = &(route->to);

  route->super.out = route->outs;
  route->super.opString = op_route_opstring;
  route->super.inString = op_route_instring;
  route->super.outString = op_route_outstring;
  route->super.type = eOpRoute;

  // class state
  route->val = 0;
  route->to = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_route_in_val(op_route_t* route, const io_t v) {
  route->val = v;
  net_activate(route->outs[route->to], route->val, route);
}

// to where
static void op_route_in_to(op_route_t* route, const io_t v) {
  io_t val = v;
  if(val<0) val = 0;
  else if(val>3) val = 3;
  route->to = val;
}


// serialization
u8* op_route_pickle(op_route_t* route, u8* dst) {
  dst = pickle_io(route->to, dst);
  return dst;
}

const u8* op_route_unpickle(op_route_t* route, const u8* src) {
  src = unpickle_io(src, &(route->to));
  return src;
}
