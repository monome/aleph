#include "net_protected.h"
#include "op_history.h"

//-------------------------------------------------
//----- static function declaration
static void op_history_in_in(op_history_t* history, const io_t v);

// pickle / unpickle
static u8* op_history_pickle(op_history_t* op, u8* dst);
static const u8* op_history_unpickle(op_history_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_history_in_fn[1] = {
  (op_in_fn)&op_history_in_in
};

static const char* op_history_instring  = "IN\0     ";
static const char* op_history_outstring = "AVG\0    O0\0     O1\0     O2\0     O3\0     O4\0     O5\0     O6\0     O7\0     ";
static const char* op_history_opstring  = "HIST";

//-------------------------------------------------
//----- external function definitions
void op_history_init(void* mem) {
  op_history_t* history = (op_history_t*)mem;
  history->super.numInputs = 1;
  history->super.numOutputs = 9;
  history->outs[0] = -1;
  history->outs[1] = -1;
  history->outs[2] = -1;
  history->outs[3] = -1;
  history->outs[4] = -1;
  history->outs[5] = -1;
  history->outs[6] = -1;
  history->outs[7] = -1;
  history->outs[8] = -1;

  history->super.in_fn = op_history_in_fn;
  history->super.pickle = (op_pickle_fn) (&op_history_pickle);
  history->super.unpickle = (op_unpickle_fn) (&op_history_unpickle);

  history->super.in_val = history->in_val;
  history->super.out = history->outs;
  history->super.opString = op_history_opstring;
  history->super.inString = op_history_instring;
  history->super.outString = op_history_outstring;
  history->super.type = eOpHistory;  
  history->in_val[0] = &(history->in);

  history->in = 0;
}

//-------------------------------------------------
//----- static function definitions
static void op_history_in_in(op_history_t* history, const io_t v) {
  // printf("history at %d received A %d\n", (int)history, (int)*v);
  s32 avg = 0;
  u8 i;

  history->in = v;

  history->val[7] = history->val[6];
  history->val[6] = history->val[5];
  history->val[5] = history->val[4];
  history->val[4] = history->val[3];
  history->val[3] = history->val[2];
  history->val[2] = history->val[1];
  history->val[1] = history->val[0];
  history->val[0] = v;

  for(i=0;i<8;i++) avg += history->val[i];
  avg = avg / 8;

  net_activate(history->outs[0], avg, history);
  net_activate(history->outs[1], history->val[0], history);
  net_activate(history->outs[2], history->val[1], history);
  net_activate(history->outs[3], history->val[2], history);
  net_activate(history->outs[4], history->val[3], history);
  net_activate(history->outs[5], history->val[4], history);
  net_activate(history->outs[6], history->val[5], history);
  net_activate(history->outs[7], history->val[6], history);
  net_activate(history->outs[8], history->val[7], history);
}


// pickle / unpickle
u8* op_history_pickle(op_history_t* op, u8* dst) {
  dst = pickle_io(op->in, dst);
  return dst;
}

const u8* op_history_unpickle(op_history_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->in));
  return src;
}
