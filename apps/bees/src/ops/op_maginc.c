#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_maginc.h"

//-------------------------------------------------
//----- descriptor
static const char* op_maginc_instring = "INC\0    ";
static const char* op_maginc_outstring = "VAL\0    ";
static const char* op_maginc_opstring = "MAGINC";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_maginc_in_inc(op_maginc_t* maginc, const io_t v);
// pickle / unpickle
static u8* op_maginc_pickle(op_maginc_t* maginc, u8* dst);
static u8* op_maginc_unpickle(op_maginc_t* maginc, const u8* src);

// array of input functions 
static op_in_fn op_maginc_in[1] = {
  (op_in_fn)&op_maginc_in_inc,
};

//----- external function definition

/// initialize
void op_maginc_init(void* op) {
  op_maginc_t* maginc = (op_maginc_t*)op;

  // superclass functions
  maginc->super.in_fn = op_maginc_in;
  maginc->super.pickle = (op_pickle_fn) (&op_maginc_pickle);
  maginc->super.unpickle = (op_unpickle_fn) (&op_maginc_unpickle);
  
  // superclass state
  maginc->super.numInputs = 1;
  maginc->super.numOutputs = 1;
  maginc->outs[0] = -1;
 
  maginc->super.in_val = maginc->in_val;
  maginc->in_val[0] = &(maginc->inc);

  maginc->super.out = maginc->outs;
  maginc->super.opString = op_maginc_opstring;
  maginc->super.inString = op_maginc_instring;
  maginc->super.outString = op_maginc_outstring;
  maginc->super.type = eOpMaginc;

}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_maginc_in_inc(op_maginc_t* maginc, const io_t v) {
  maginc->inc = v;
  if(maginc->outs[0] >= 0 && maginc->outs[0] < net->numIns) {
    // if it's an op input:
    inode_t *targetIn = &net->ins[maginc->outs[0]];
    io_t targetInVal = *net->ops[targetIn->opIdx]->in_val[targetIn->opInIdx];
    /* print_dbg("\r\noutIdx: "); */
    /* print_dbg_ulong(maginc->outs[0]); */
    /* print_dbg("\r\nopIdx: "); */
    /* print_dbg_ulong(targetIn->opIdx); */
    /* print_dbg("\r\nopInIdx: "); */
    /* print_dbg_ulong(targetIn->opInIdx); */
    net_activate(maginc, 0, op_sadd(targetInVal, v));
  } else if ( maginc->outs[0] >= net->numIns) {
    // if it's a DSP param:
    ParamData targetParamData = net->params[maginc->outs[0] - net->numIns].data;
    net_activate(maginc, 0, op_sadd(targetParamData.value, v));
  }
}

// serialization
u8* op_maginc_pickle(op_maginc_t* maginc, u8* dst) {
  return dst;
}

u8* op_maginc_unpickle(op_maginc_t* maginc, const u8* src) {
   return (u8*)src;
}
