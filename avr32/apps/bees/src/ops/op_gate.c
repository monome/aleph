#include "net_protected.h"
#include "op_gate.h"

//-------------------------------------------------
//----- static function declaration
static void op_gate_in_value(op_gate_t* gate, const io_t* v);
static void op_gate_in_gate(op_gate_t* gate, const io_t* v);
static void op_gate_in_store(op_gate_t* gate, const io_t* v);
static void op_gate_inc_input(op_gate_t* gate, const s16 idx, const io_t inc);

//-------------------------------------------------
//----- static vars
static const char* op_gate_instring = "VALUE   GATE    STORE   ";
static const char* op_gate_outstring = "GATED   ";
static const char* op_gate_opstring = "GATE";

static op_in_func_t op_gate_in_func[3] = {
  (op_in_func_t)&op_gate_in_value,
  (op_in_func_t)&op_gate_in_gate, 
  (op_in_func_t)&op_gate_in_store
};

//-------------------------------------------------
//----- external function definitions
void op_gate_init(void* mem) {
  op_gate_t* gate = (op_gate_t*)mem;
  gate->super.numInputs = 3;
  gate->super.numOutputs = 1;
  gate->outs[0] = -1;
  gate->super.inc_func = (op_inc_func)op_gate_inc_input;
  gate->super.in_func = op_gate_in_func;
  gate->super.in_val = gate->in_val;
  gate->super.out = gate->outs;
  gate->super.opString = op_gate_opstring;
  gate->super.inString = op_gate_instring;
  gate->super.outString = op_gate_outstring;
  gate->super.type = eOpGate;  
  //gate->super.status = eUserOp;

  gate->in_val[0] = &(gate->val);
  gate->in_val[1] = &(gate->gate);
  gate->in_val[2] = &(gate->store);
}

//-------------------------------------------------
//----- static function definitions
static void op_gate_in_value(op_gate_t* gate, const io_t* v) {
  gate->val = *v;
  if(gate->gate != 0) {
    net_activate(gate->outs[0], gate->val, gate);
  }
}

static void op_gate_in_gate(op_gate_t* gate, const io_t* v) {
  //  gate->gate = (io_t)(*v != 0);
  if(*v > 0) { gate->gate = 1; } else { gate->gate = 0; }
  if (gate->store) {
    net_activate(gate->outs[0], gate->val, gate);
  }
}

static void op_gate_in_store(op_gate_t* gate, const io_t* v) {
  //  gate->store = (*v != 0);
  if(*v > 0) { gate->store = 1; } else { gate->store = 0; }
}

static void op_gate_inc_input(op_gate_t* gate, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0:  // value
      val = OP_ADD(gate->val, inc);
      op_gate_in_value(gate, &val);
    break; 
  case 1:  // gate
    op_gate_in_gate(gate, &inc);
    break;
  case 2:  // storage mode
    op_gate_in_store(gate, &inc);
    break;
  }
}
