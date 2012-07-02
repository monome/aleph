/* net.c
 * bees
 * aleph
 *
 * definition of a network of control operators
 * and points of IO connection between them
 */

#include <stdio.h>

#include "../common/types.h"
#include "util.h"
#include "op.h"
#include "param.h"
#include "net.h"
#include "net_protected.h"

//=========================================
//===== variables
//----- external
ctlnet_t net;

//==================================================
//========= static functions

// create all system operators
static void add_sys_ops(void);
static void add_sys_ops(void) {
  // 4 encoders
  net_add_op(eOpEnc);
  net_add_op(eOpEnc);
  net_add_op(eOpEnc);
  net_add_op(eOpEnc);
  // 4 switches
  net_add_op(eOpSwitch);
  net_add_op(eOpSwitch);
  net_add_op(eOpSwitch);
  net_add_op(eOpSwitch);
  // 1 param receiver
  //// no, params are specialized input nodes
  //net_add_op(eOpParam);
  // 1 preset receiver
  net_add_op(eOpPreset);
}

//==================================================
//========= public functions

// "singleton" intializer
void net_init(void) {
  U32 i;
  for(i=0; i<NET_OP_POOL_SIZE; i++) {
    net.opPoolMem[i] = (U8)0;
  }
  net.opPool = (void*)&(net.opPoolMem);
  net.numOps = 0;
  net.numIns = 0;
  net.numOuts = 0;
  net.numParams = 0;
  net.opPoolOffset = 0;
  printf("initialized network, using %d bytes\n", (int)sizeof(ctlnet_t));
  add_sys_ops();
  
}

// de-initialize network
void net_deinit(void) {
  ;;
}

// activate an input node with a value
void net_activate(S16 inIdx, const S32* val) {
  if(inIdx >= 0) {
    if(inIdx < net.numIns) {
      (*(net.ins[inIdx].in))(net.ops[net.ins[inIdx].opIdx], val);
    } else { 
      // index in dsp param list
      inIdx -= net.numIns;
      if (inIdx >= net.numParams) {
	return ;
      } else {
	set_param_value(inIdx, *val);
      }
    }
  }  
}

// attempt to allocate a new operator from the pool, return index
S16 net_add_op(opId_t opId) {
  U16 ins, outs;
  U8 i;
  op_t* op;

  if (net.numOps >= NET_OPS_MAX) {
    return -1;
  }

  if (op_registry[opId].size > NET_OP_POOL_SIZE - net.opPoolOffset) {
    return -1;
  }

  op = (op_t*)(net.opPool + net.opPoolOffset);
  // use the class ID to initialize a new object in scratch
  switch(opId) {
  case eOpSwitch:
    op_sw_init((void*) op);
    break;
  case eOpEnc:
    op_enc_init((void*)op);
    break;
    /*
  case eOpParam:
    op_param_init((void*)op);
    break;
    */
  case eOpPreset:
    //    return -1;
    break;
  case eOpAdd:
    op_add_init((void*)op);
    break;
  case eOpMul:
    op_mul_init((void*)op);
    break;
  case eOpGate:
    op_gate_init((void*)op);
    break;
  case eOpAccum:
    op_accum_init((void*)op);
    break;
  case eOpSelect:
    return -1;
    break;
  case eOpMapLin:
    return -1;
    break;
  default:
    return -1;
  }

  ins = op->numInputs;
  outs = op->numOutputs;
  op->type = opId;
 
  if (ins > (NET_INS_MAX - net.numIns)) {
    return -1;
  }

  if (outs > (NET_OUTS_MAX - net.numOuts)) {
    return -1;
  }

  // add op pointer to list
  net.ops[net.numOps] = op;
  net.opPoolOffset += op_registry[opId].size;

  //---- add inputs and outputs to node list
  // don't add inputs for system control ops
  if (op->status != eSysCtlOp) {
    for(i=0; i<ins; i++) {
      net.ins[net.numIns].in = op->in[i];
      net.ins[net.numIns].opIdx = net.numOps;
      net.ins[net.numIns].inIdx = i;
      net.numIns++;
    }
  }
  for(i=0; i<outs; i++) {
    net.outs[net.numOuts].opIdx = net.numOps;
    net.outs[net.numOuts].outIdx = i;
    net.outs[net.numOuts].target = -1;
    net.numOuts++;
  }
  net.numOps++;
  return net.numOps - 1;
}

// destroy last operator created
S16 net_pop_op(void) {
  op_t* op = net.ops[net.numOps - 1];
  net.numIns -= op->numInputs;
  net.numOuts -= op->numOutputs;
  net.opPoolOffset += op_registry[op->type].size;
  net.numOps -= 1;
  return 0;
}

// create a connection between given idx pair
void net_connect(U32 oIdx, U32 iIdx) {
  net.ops[net.outs[oIdx].opIdx]->out[net.outs[oIdx].outIdx] = iIdx;
  net.outs[oIdx].target = iIdx;
}

// disconnect given output
void net_disconnect(U32 outIdx) {
  net.ops[net.outs[outIdx].opIdx]->out[net.outs[outIdx].outIdx] = -1;
  net.outs[outIdx].target = -1;
}

//---- queries

// get current count of operators
U16 net_num_ops(void) {
  return net.numOps;
}

// get current count of inputs
U16 net_num_ins(void) {
  return net.numIns + net.numParams;
}

// get current count of outputs
U16 net_num_outs(void) {
  return net.numOuts;
}

// get param index given input index
S16 net_param_idx(U16 inIdx) {
  return inIdx - net.numIns;
}

// get string for operator at given idx
const char* net_op_name(const S16 idx) {
  if (idx < 0) {
    return "";
  }
  return net.ops[idx]->opString;
}

// get name for input at given idx
const char* net_in_name(U16 idx) {
  if (idx >= net.numIns) {
    // not an operator input
    idx -= net.numIns;
    if (idx >= net.numParams) {
      // not a param input either
      return "";
    } else {
      // param input
      return net.params[idx].name;
    }
  } else {
    // op input
    return op_in_name(net.ops[net.ins[idx].opIdx], net.ins[idx].inIdx);
  }
}

// get name for output at given idx
const char* net_out_name(const U16 idx) {
  return op_out_name(net.ops[net.outs[idx].opIdx], net.outs[idx].outIdx);
}

// get op index for input at given idx
S16 net_in_op_idx(const U16 idx) {
  if (idx >= net.numIns) return -1;
  return net.ins[idx].opIdx;
}

// get op index for output at given idx
S16 net_out_op_idx(const U16 idx) {
 if (idx > net.numOuts) return -1;
  return net.outs[idx].opIdx;
}


// get global index for a given input of given op
U16 net_op_in_idx(const U16 opIdx, const U16 inIdx) {
  U16 which;
  for(which=0; which<net.numIns; which++) {
    if(net.ins[which].opIdx == opIdx) {
      return (which + inIdx);
    }
  }
  return 0; // shouldn't get here
}

// get global index for a given output of given op
U16 net_op_out_idx(const U16 opIdx, const U16 outIdx) {
  U16 which;
  for(which=0; which<net.numOuts; which++) {
    if(net.outs[which].opIdx == opIdx) {
      return (which + outIdx);
    }
  }
  return 0; // shouldn't get here
}

// get connection index for output
S16 net_get_target(U16 outIdx) {
  return net.outs[outIdx].target;
}

// is this input connected to anything?
U8 net_in_connected(U32 iIdx) {
  U8 f=0;
  U16 i;
  for(i=0; i<net.numOuts; i++) {
    if(net.outs[i].target == iIdx) {
      f = 1;
      break;
    }
  }
  return f;
}

// get status (user/system) of op at given idx
opStatus_t net_op_status(U16 opIdx) {
  return net.ops[opIdx]->status;
}

// populate an array with indices of all connected outputs for a given index
// returns count of connections
U32 net_gather(U32 iIdx, U32(*outs)[NET_OUTS_MAX]) {
  U32 iTest;
  U32 iOut=0;
  for(iTest=0; iTest<NET_OUTS_MAX; iTest++) {
    if(net.outs[iTest].target == iIdx) {
      (*outs)[iOut] = iTest;
      iOut++;
    }
  }
  return iOut;
}
//--- get / set / increment input value
f32 net_get_in_value(U16 inIdx) {
  if (inIdx >= net.numIns) {
    inIdx -= net.numIns;
    return net.params[inIdx].val;
  } else {
    return net.ins[inIdx].val;
  }
}

void net_set_in_value(U16 inIdx, S32 val) {
  net.ins[inIdx].val = val;
}

f32 net_inc_in_value(U16 inIdx, S32 inc) {
  net.ins[inIdx].val += inc;
  net_activate(inIdx, (const S32*)(&(net.ins[inIdx].val)));
  return net.ins[inIdx].val;
}

// toggle preset inclusion for input
u8 net_toggle_in_preset(U32 inIdx) {
  net.ins[inIdx].preset ^= 1;
  return net.ins[inIdx].preset;
}

// toggle preset inclusion for output
u8 net_toggle_out_preset(U32 outIdx) {
  net.outs[outIdx].preset ^= 1;
  return net.outs[outIdx].preset;
}

// get preset inclusion for input
U8 net_get_in_preset(U32 inIdx) {
  return net.ins[inIdx].preset;
}

// get preset inclusion for output
U8 net_get_out_preset(U32 outIdx) {
  return net.outs[outIdx].preset;
}

//------------------------------------
//------ params

// add a new parameter
void net_add_param(u32 idx, const char* name, f32 min, f32 max, f32 val) {
  net.params[net.numParams].idx = idx;
  net.params[net.numParams].name = name;
  net.params[net.numParams].min = min;
  net.params[net.numParams].max = max;
  net.params[net.numParams].val = val;
  net.numParams++;
}

// clear existing parameters
void net_clear_params(void) {
  // u32 i;
  // for(i=0; i<net.numParams; i++) {
   
  //  }
  net.numParams = 0;
}
