/* ctl_network.c
 * aleph-avr32
 *
 * definition of a network of control operators
 * and points of IO connection between them
 */

#include <stdio.h>

#include "compiler.h"
#include "util.h"
#include "op.h"
#include "ctlnet_interface.h"

// total size of operator pool!
#define CTLNET_OP_POOL_SIZE 8192

//====================================================
//============= data types

// ------ node types
// in
typedef struct inode_struct { 
  op_in_t in; // function pointer
  S32 val; // value for scene management
  U8 opIdx; // index of parent op in oplist
  U8 inIdx; // index of input in parent op's inlist
  U8 scene; // flag if included in scene
} inode_t;

// out
typedef struct onode_struct { 
  U8 opIdx;   // index of parent op in oplist
  U8 outIdx;  // index of this output in parent op's outlist
  U8 target;     // target index in inodes
} onode_t;

//------- network type
typedef struct ctlnet_struct {
  // count of operators
  U16 numOps;
  // count of inputs
  U16 numIns;
  // count of outputs
  U16 numOuts;
  // list of operator pointers
  op_t* ops[CTLNET_OPS_MAX];
  // list of inputs
  inode_t ins[CTLNET_INS_MAX];
  // list of outputs
  onode_t outs[CTLNET_OUTS_MAX];
  // operator memory pool
  U8 opPoolMem[CTLNET_OP_POOL_SIZE];
  // pointer to access the pool
  void* opPool;
  // current offset to free space within the pool 
  U32 opPoolOffset;
} ctlnet_t;

//====================================
//============ static variables
static ctlnet_t net;

//==================================================
//========= public functions

// "singleton" intializer
void net_init(void) {
  U32 i;
  for(i=0; i<CTLNET_OP_POOL_SIZE; i++) {
    net.opPoolMem[i] = (U8)0;
  }
  net.opPool = (void*)&(net.opPoolMem);
  net.numOps = 0;
  net.numIns = 0;
  net.numOuts = 0;
  net.opPoolOffset = 0;
  printf("initialized network, using %d bytes\n", (S32)sizeof(ctlnet_t));
}

// activate an input node by calling its function pointer
void net_activate(S16 inIdx, const S32* val) {
  if(inIdx >= 0) {
    (*(net.ins[inIdx].in))(net.ops[net.ins[inIdx].opIdx], val);
  }
}

// attempt to allocate a new operator from the pool, return index
S16 net_add_op(opid_t opId) {
  U8 idx;
  U8 ins;
  U8 outs;
  U8 i;
  op_t* op;

  if (net.numOps >= CTLNET_OPS_MAX) {
    return -1;
  }

  if (op_registry[opId].size > CTLNET_OP_POOL_SIZE - net.opPoolOffset) {
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
  case eOpParam:
    return -1;
    break;
  case eOpPreset:
    return -1;
    break;
  default:
    return -1;
  }

  ins = op->numInputs;
  outs = op->numOutputs;
  op->type = opId;
 
  if (ins > (CTLNET_INS_MAX - net.numIns)) {
    return -1;
  }

  if (outs > (CTLNET_OUTS_MAX - net.numOuts)) {
    return -1;
  }

  // add op pointer to list
  net.ops[net.numOps] = op;
  net.opPoolOffset += op_registry[opId].size;

 // add inputs and outputs to node list
  for(i=0; i<ins; i++) {
    net.ins[net.numIns].in = op->in[i];
    net.ins[net.numIns].opIdx = net.numOps;
    net.ins[net.numIns].inIdx = i;
    net.numIns++;
  }
  for(i=0; i<outs; i++) {
    net.outs[net.numOuts].opIdx = net.numOps;
    net.outs[net.numOuts].outIdx = i;
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
}

// create a connection between given idx pair
void net_connect(U32 oIdx, U32 iIdx) {
  net.ops[net.outs[oIdx].opIdx]->out[net.outs[oIdx].outIdx] = iIdx;
}

// disconnect given output
void net_disconnect(U32 outIdx) {
  net.ops[net.outs[outIdx].opIdx]->out[net.outs[outIdx].outIdx] = -1;
}

//---- queries

// get current count of operators
U8 net_num_ops(void) {
  return net.numOps;
}

// get current count of inputs
U8 net_num_ins(void) {
  return net.numIns;
}

// get current count of outputs
U8 net_num_outs(void) {
  return net.numOuts;
}

// get string for operator at given idx
const char* net_op_name(const U8 idx) {
  return net.ops[idx]->opString;
}
// get name for input at given idx
const char* net_in_name(const U8 idx) {
  return op_in_name(net.ops[net.ins[idx].opIdx], net.ins[idx].inIdx);
}

// get name for output at given idx
const char* net_out_name(const U8 idx) {
  return op_out_name(net.ops[net.outs[idx].opIdx], net.outs[idx].outIdx);
}

// get op index for input at given idx
U8 net_in_op_idx(const U8 idx) {
  return net.ins[idx].opIdx;
}

// get op index for output at given idx
U8 net_out_op_idx(const U8 idx) {
  return net.outs[idx].opIdx;
}

// populate an array with indices of all connected outputs for a given index
// returns count of connections
U32 net_gather(U32 iIdx, U32(*outs)[CTLNET_OUTS_MAX]) {
  U32 iTest;
  U32 iOut=0;
  for(iTest=0; iTest<CTLNET_OUTS_MAX; iTest++) {
    if(net.outs[iTest].target == iIdx) {
      (*outs)[iOut] = iTest;
      iOut++;
    }
  }
  return iOut;
}

