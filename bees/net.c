/* net.c
 * bees
 * aleph
 *
 * definition of a network of control operators
 * and points of IO connection between them
 */

#include <stdio.h>

#include "types.h"
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
  //  net_add_op(eOpPreset);
}

//==================================================
//========= public functions

// "singleton" intializer
void net_init(void) {
  u32 i;
  for(i=0; i<NET_OP_POOL_SIZE; i++) {
    net.opPoolMem[i] = (u8)0;
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
void net_activate(s16 inIdx, const io_t val) {
  if(inIdx >= 0) {
    if(inIdx < net.numIns) {
      //(*(net.ins[inIdx].in))(net.ops[net.ins[inIdx].opIdx], val);
      op_set_in_val(net.ops[net.ins[inIdx].opIdx],
		    net.ins[inIdx].opInIdx,
		    val);
    } else { 
      // index in dsp param list
      inIdx -= net.numIns;
      if (inIdx >= net.numParams) {
	return ;
      } else {
	set_param_value(inIdx, val);
      }
    }
  }  
}

// attempt to allocate a new operator from the pool, return index
s16 net_add_op(opId_t opId) {
  u16 ins, outs;
  u8 i;
  op_t* op;

  if (net.numOps >= NET_OPS_MAX) {
    return -1;
  }

  if (op_registry[opId].size > NET_OP_POOL_SIZE - net.opPoolOffset) {
    return -1;
  }

  op = (op_t*)((u8*)net.opPool + net.opPoolOffset);
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
#if 0
  case eOpPreset:
    //    return -1;
    break;
#endif
  case eOpAdd:
    op_add_init((void*)op);
    break;
  case eOpMul:
    op_mul_init((void*)op);
    break;
  case eOpGate:
    op_gate_init((void*)op);
    break;
#if 0
  case eOpAccum:
    op_accum_init((void*)op);
    break;
  case eOpSelect:
    return -1;
    break;
  case eOpMapLin:
    return -1;
    break;
#endif
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
  //no // don't add inputs for system control ops
  // yes actually do
  //if (op->status != eSysCtlOp) {
    for(i=0; i<ins; i++) {
      //      net.ins[net.numIns].in = op->in[i];
      net.ins[net.numIns].opIdx = net.numOps;
      net.ins[net.numIns].opInIdx = i;
      //      net.ins[net.numIns].netInIdx = net.numIns;
      net.numIns++;
    }
    //}
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
s16 net_pop_op(void) {
  op_t* op = net.ops[net.numOps - 1];
  net.numIns -= op->numInputs;
  net.numOuts -= op->numOutputs;
  net.opPoolOffset += op_registry[op->type].size;
  net.numOps -= 1;
  return 0;
}

// create a connection between given idx pair
void net_connect(u32 oIdx, u32 iIdx) {
  net.ops[net.outs[oIdx].opIdx]->out[net.outs[oIdx].outIdx] = iIdx;
  net.outs[oIdx].target = iIdx;
}

// disconnect given output
void net_disconnect(u32 outIdx) {
  net.ops[net.outs[outIdx].opIdx]->out[net.outs[outIdx].outIdx] = -1;
  net.outs[outIdx].target = -1;
}

//---- queries

// get current count of operators
u16 net_num_ops(void) {
  return net.numOps;
}

// get current count of inputs
u16 net_num_ins(void) {
  return net.numIns + net.numParams;
}

// get current count of outputs
u16 net_num_outs(void) {
  return net.numOuts;
}

// get num params (subset of inputs)
u16 net_num_params(void) {
  return net.numParams;
}

// get param index given input index
s16 net_param_idx(u16 inIdx) {
  return inIdx - net.numIns;
}

// get string for operator at given idx
const char* net_op_name(const s16 idx) {
  if (idx < 0) {
    return "";
  }
  return net.ops[idx]->opString;
}

// get name for input at given idx
const char* net_in_name(u16 idx) {
  if (idx >= net.numIns) {
    // not an operator input
    idx -= net.numIns;
    if (idx >= net.numParams) {
      // not a param input either
      return "";
    } else {
      // param input
      return net.params[idx].desc.label;
    }
  } else {
    // op input
    return op_in_name(net.ops[net.ins[idx].opIdx], net.ins[idx].opInIdx);
  }
}

// get name for output at given idx
const char* net_out_name(const u16 idx) {
  return op_out_name(net.ops[net.outs[idx].opIdx], net.outs[idx].outIdx);
}

// get op index for input at given idx
s16 net_in_op_idx(const u16 idx) {
  if (idx >= net.numIns) return -1;
  return net.ins[idx].opIdx;
}

// get op index for output at given idx
s16 net_out_op_idx(const u16 idx) {
 if (idx > net.numOuts) return -1;
  return net.outs[idx].opIdx;
}


// get global index for a given input of given op
u16 net_op_in_idx(const u16 opIdx, const u16 inIdx) {
  u16 which;
  for(which=0; which<net.numIns; which++) {
    if(net.ins[which].opIdx == opIdx) {
      return (which + inIdx);
    }
  }
  return 0; // shouldn't get here
}

// get global index for a given output of given op
u16 net_op_out_idx(const u16 opIdx, const u16 outIdx) {
  u16 which;
  for(which=0; which<net.numOuts; which++) {
    if(net.outs[which].opIdx == opIdx) {
      return (which + outIdx);
    }
  }
  return 0; // shouldn't get here
}

// get connection index for output
s16 net_get_target(u16 outIdx) {
  return net.outs[outIdx].target;
}

// is this input connected to anything?
u8 net_in_connected(s32 iIdx) {
  u8 f=0;
  u16 i;
  for(i=0; i<net.numOuts; i++) {
    if(net.outs[i].target == iIdx) {
      f = 1;
      break;
    }
  }
  return f;
}

// get status (user/system) of op at given idx
opStatus_t net_op_status(u16 opIdx) {
  return net.ops[opIdx]->status;
}

// populate an array with indices of all connected outputs for a given index
// returns count of connections
u32 net_gather(s32 iIdx, u32(*outs)[NET_OUTS_MAX]) {
  u32 iTest;
  u32 iOut=0;
  for(iTest=0; iTest<NET_OUTS_MAX; iTest++) {
    if(net.outs[iTest].target == iIdx) {
      (*outs)[iOut] = iTest;
      iOut++;
    }
  }
  return iOut;
}
//--- get / set / increment input value
io_t net_get_in_value(s32 inIdx) {
  if (inIdx >= net.numIns) {
    inIdx -= net.numIns;
    return get_param_value(inIdx);
  } else {
    //return (net.ins[inIdx].val);
    return op_get_in_val(net.ops[net.ins[inIdx].opIdx], net.ins[inIdx].opInIdx);
  }
}

void net_set_in_value(s32 inIdx, io_t val) {
  //net.ins[inIdx].val = val;
  if (inIdx < 0) return;
  if (inIdx < net.numIns) {
    op_set_in_val(net.ops[net.ins[inIdx].opIdx], net.ins[inIdx].opInIdx, val);
  } else {
    // parameter
    inIdx -= net.numIns;
    set_param_value(inIdx, val);
  }
}

io_t net_inc_in_value(s32 inIdx, io_t inc) {
  if (inIdx >= net.numIns) {
    inIdx -= net.numIns;
    set_param_value(inIdx, OPADD(get_param_value(inIdx), inc));
    return get_param_value(inIdx);
  } else {
    //net.ins[inIdx].val += inc;
    //net_activate(inIdx, (const s32*)(&(net.ins[inIdx].val)));
    //return net.ins[inIdx].
    net_activate( inIdx, 
		  OPADD(
			op_get_in_val( net.ops[net.ins[inIdx].opIdx],
				       net.ins[inIdx].opInIdx ), inc));
    return op_get_in_val( net.ops[net.ins[inIdx].opIdx],
			    net.ins[inIdx].opInIdx ); 
  }
}

// toggle preset inclusion for input
u8 net_toggle_in_preset(u32 id) {
  net.ins[id].preset ^= 1;
  return net.ins[id].preset;
}

// toggle preset inclusion for output
u8 net_toggle_out_preset(u32 id) {
  net.outs[id].preset ^= 1;
  return net.outs[id].preset;
}

// set preset inclusion for input
void net_set_in_preset(u32 id, u8 val) {
  net.ins[id].preset = val;
}

// set preset inclusion for output
void net_set_out_preset(u32 outIdx, u8 val) {
 net.outs[outIdx].preset = val;
}

// get preset inclusion for input
u8 net_get_in_preset(u32 id) {
  return net.ins[id].preset;
}

// get preset inclusion for output
u8 net_get_out_preset(u32 id) {
  return net.outs[id].preset;
}

//------------------------------------
//------ params

// add a new parameter
//void net_add_param(u32 idx, const char* name, f32 min, f32 max, s32 val) {
void net_add_param(u32 idx, ParamDesc* pdesc) {
  net.params[net.numParams].desc = *pdesc;
  net.params[net.numParams].idx = idx;
  net.params[net.numParams].preset = 1;
  net.numParams++;
}

// clear existing parameters
void net_clear_params(void) {
  // u32 i;
  // for(i=0; i<net.numParams; i++) {
   
  //  }
  net.numParams = 0;
}
