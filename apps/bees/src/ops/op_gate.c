#include "net_protected.h"
#include "op_gate.h"

//-------------------------------------------------
//----- static function declaration
static void op_gate_in_value(op_gate_t* gate, const io_t v);
static void op_gate_in_gate(op_gate_t* gate, const io_t v);
static void op_gate_in_store(op_gate_t* gate, const io_t v);

//-------------------------------------------------
//----- static vars
static const char* op_gate_instring = "VAL\0    GATE\0   STORE\0  ";
static const char* op_gate_outstring = "GATED   ";
static const char* op_gate_opstring = "GATE";

static op_in_fn op_gate_in_fn[3] = {
  (op_in_fn)&op_gate_in_value,
  (op_in_fn)&op_gate_in_gate, 
  (op_in_fn)&op_gate_in_store
};

//-------------------------------------------------
//----- external function definitions
void op_gate_init(void* mem) {
  op_gate_t* gate = (op_gate_t*)mem;
  gate->super.numInputs = 3;
  gate->super.numOutputs = 1;
  gate->outs[0] = -1;
  gate->super.in_fn = op_gate_in_fn;
  gate->super.in_val = gate->in_val;
  gate->super.out = gate->outs;
  gate->super.opString = op_gate_opstring;
  gate->super.inString = op_gate_instring;
  gate->super.outString = op_gate_outstring;
  gate->super.type = eOpGate;  

  gate->in_val[0] = &(gate->val);
  gate->in_val[1] = &(gate->gate);
  gate->in_val[2] = &(gate->store);
}

//-------------------------------------------------
//----- static function definitions
static void op_gate_in_value(op_gate_t* gate, const io_t v) {
  gate->val = v;
  if(gate->gate != 0) {
    net_activate(gate->outs[0], gate->val, gate);
  }
}

static void op_gate_in_gate(op_gate_t* gate, const io_t v) {
  if(v > 0) { gate->gate = 1; } else { gate->gate = 0; }
  if (gate->store) {
    // in storage mode, 2nd input emits stored value
    net_activate(gate->outs[0], gate->val, gate);
  }
}

static void op_gate_in_store(op_gate_t* gate, const io_t v) {
  //  gate->store = (*v != 0);
  if(v > 0) { gate->store = 1; } else { gate->store = 0; }
}
