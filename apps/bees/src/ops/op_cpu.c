#include "net_protected.h"
#include "bfin.h"
#include "op_cpu.h"

//-------------------------------------------------
//----- static function declaration
static void op_cpu_in_trig(op_cpu_t* op, const io_t v);

static u8* op_cpu_pickle(op_cpu_t* op, u8* dst);
static const u8* op_cpu_unpickle(op_cpu_t* op, const u8* src);

//-------------------------------------------------
//----- static vars

// input function table
static op_in_fn op_cpu_in_fn[1] = {
  (op_in_fn)&op_cpu_in_trig
};

static const char* op_cpu_opstring  = "CPU";
static const char* op_cpu_instring  = "TRIG\0   ";
static const char* op_cpu_outstring = "AUDIO\0  CONTROL\0";

//-------------------------------------------------
//----- external function definitions

// initialize at memory location
void op_cpu_init(void* mem) {
  op_cpu_t* op = (op_cpu_t*)mem;
  op->super.numInputs = 1;
  op->super.numOutputs = 2;
  op->outs[0] = -1;
  op->outs[1] = -1;

  op->super.in_fn = op_cpu_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_cpu_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_cpu_unpickle);

  op->super.in_val = op->in_val;
  op->super.in_val = op->in_val;
  op->super.out = op->outs;
  op->super.opString = op_cpu_opstring;
  op->super.inString = op_cpu_instring;
  op->super.outString = op_cpu_outstring;
  op->super.type = eOpCpu;  
  op->in_val[0] = &(op->trig);

  
}



// update outputs with CPU usage counts from blackfin
static void op_cpu_in_trig(op_cpu_t* op, const io_t v) {
  io_t audioCycles;
  io_t controlCycles;
  audioCycles = (io_t)bfin_get_audio_cpu();
  controlCycles = (io_t)bfin_get_control_cpu();
  net_activate(op, 0, audioCycles);
  net_activate(op, 1, controlCycles);
}


// pickle / unpickle
u8* op_cpu_pickle(op_cpu_t* op, u8* dst) {
  return dst;
}

const u8* op_cpu_unpickle(op_cpu_t* op, const u8* src ) {
  return src;
}
