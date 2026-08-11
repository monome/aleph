#include "net_protected.h"
#include "op_hist2.h"

//-------------------------------------------------
//----- static function declaration
static void op_hist2_in_in(op_hist2_t* history, const io_t v);

// pickle / unpickle
static u8* op_hist2_pickle(op_hist2_t* op, u8* dst);
static const u8* op_hist2_unpickle(op_hist2_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_hist2_in_fn[1] = {
  (op_in_fn)&op_hist2_in_in
};

static const char* op_hist2_instring  = "IN\0     ";
static const char* op_hist2_outstring = "O0\0     O1\0     ";
static const char* op_hist2_opstring  = "HIST2";

//-------------------------------------------------
//----- external function definitions
void op_hist2_init(void* mem) {
  op_hist2_t* history = (op_hist2_t*)mem;
  history->super.numInputs = 1;
  history->super.numOutputs = 2;
  history->outs[0] = -1;
  history->outs[1] = -1;

  history->super.in_fn = op_hist2_in_fn;
  history->super.pickle = (op_pickle_fn) (&op_hist2_pickle);
  history->super.unpickle = (op_unpickle_fn) (&op_hist2_unpickle);

  history->super.in_val = history->in_val;
  history->super.out = history->outs;
  history->super.opString = op_hist2_opstring;
  history->super.inString = op_hist2_instring;
  history->super.outString = op_hist2_outstring;
  history->super.type = eOpHist2;  
  history->in_val[0] = &(history->in);

  history->in = 0;
}

//-------------------------------------------------
//----- static function definitions
static void op_hist2_in_in(op_hist2_t* history, const io_t v) {
  // printf("history at %d received A %d\n", (int)history, (int)*v);

  history->in = v;

  history->val[1] = history->val[0];
  history->val[0] = v;

  net_activate(history, 0, history->val[0]);
  net_activate(history, 1, history->val[1]);
}


// pickle / unpickle
u8* op_hist2_pickle(op_hist2_t* op, u8* dst) {
  dst = pickle_io(op->in, dst);
  return dst;
}

const u8* op_hist2_unpickle(op_hist2_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->in));
  return src;
}
