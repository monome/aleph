/* ctl_network.c
 * aleph-avr32
 *
 * definition of a network of control operators
 * and points of IO connection between them
 */

#include <stdio.h>

#include "compiler.h"
#include "util.h"
#include "ctl_op.h"
#include "ctl_interface.h"

// maximum allocated and operators
#define CTL_INS_MAX 128
#define CTL_OUTS_MAX 128
#define CTL_OPS_MAX 128
#define CTL_OP_POOL_SIZE 8192

//====================================================
//============= data types

// ------ node types
// in: p to operator, p to input function
typedef struct ctl_inode_struct { 
  //  ctl_op_t* op;
  U8 opIdx;
  ctl_in_t in;
} ctl_inode_t;

// out: p to op, idx in op's output array
typedef struct ctl_onode_struct { 
  //  ctl_op_t* op;
  U8 opIdx;
  U8 out;
} ctl_onode_t;

//------- network type
typedef struct ctl_net_struct {
  // count of operators
  U16 numOps;
  // count of inputs
  U16 numIns;
  // count of outputs
  U16 numOuts;
  // list of operator pointers
  ctl_op_t* ops[CTL_OPS_MAX];
  // list of inputs
  ctl_inode_t ins[CTL_INS_MAX];
  // list of outputs
  ctl_onode_t outs[CTL_OUTS_MAX];
  // operator memory pool
  U8 opPoolMem[CTL_OP_POOL_SIZE];
  // pointer to access the pool
  void* opPool;
  // current offset to free space within the pool 
  U32 opPoolOffset;
} ctl_net_t;

//====================================
//============ static variables
static ctl_net_t net;


//==================================================
//========= public functions

// "singleton" intializer
void ctl_net_init(void) {
  U32 i;
  for(i=0; i<CTL_OP_POOL_SIZE; i++) {
    net.opPoolMem[i] = (U8)0;
  }
  net.opPool = (void*)&(net.opPoolMem);
  net.numOps = 0;
  net.numIns = 0;
  net.numOuts = 0;
  net.opPoolOffset = 0;
  printf("initialized network, using %d bytes\n", sizeof(ctl_net_t));
}

// activate an input node by calling its function pointer
void ctl_go(U16 inIdx, const S32* val) {
  (*(net.ins[inIdx].in))(net.ops[net.ins[inIdx].opIdx], val);
}

// attempt to allocate a new operator from the pool, return index
S16 ctl_add_op(opid_t opId) {
  static U8 scratch[CTL_OP_MAX_SIZE];
  U8 idx;
  U8 size;
  U8 ins;// = op->getNumInputs;
  U8 outs;// = op->getNumOutputs;
  U8 i;
  ctl_op_t* op;

  if (net.numOps >= CTL_OPS_MAX) {
    return -1;
  }

  // use the class ID to initialize a new object in scratch
  switch(opId) {
  case eOpSwitch:
    op_sw_init((void*) scratch);
    break;
  case eOpEnc:
    return -1;
    break;
  case eOpAdd:
    return -1;
    break;
  case eOpMul:
    return -1;
    break;
  case eOpAccum:
    return -1;
    break;
  case eOpGate:
    return -1;
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

  size = ((ctl_op_t*)scratch)->size;
  ins = ((ctl_op_t*)scratch)->numInputs;
  outs = ((ctl_op_t*)scratch)->numOutputs;
  
  if (size > CTL_OP_POOL_SIZE - net.opPoolOffset) {
    return -1;
  }
 
  if (ins > (CTL_INS_MAX - net.numIns)) {
    return -1;
  }

  if (outs > (CTL_OUTS_MAX - net.numOuts)) {
    return -1;
  }
  
  // copy scratch to op pool (1 byte at a time...)
  for(i=0; i<(((ctl_op_t*)scratch)->size); i++) {
    net.opPoolMem[net.opPoolOffset + i] = scratch[i];
  }
 
  op = (ctl_op_t*)(net.opPool + net.opPoolOffset);
  // add op pointer to list
  net.ops[net.numOps] = op;
  net.opPoolOffset += size;

 // add inputs and outputs to node list
  for(i=0; i<ins; i++) {
    net.ins[net.numIns].in = op->in[i];
    net.ins[net.numIns].opIdx = net.numOps;
    net.numIns++;
  }
  for(i=0; i<outs; i++) {
    net.outs[net.numOuts].out = op->out[i];
    net.outs[net.numOuts].opIdx = net.numOps;
    net.numOuts++;
  }
  net.numOps++;
  return net.numOps - 1;
}

// destroy last operator created
// void ctl_remove_op(U32 opIdx);

// create a connection between given idx pair
// void ctl_connect(U32 outIdx, U32 inIdx);

// disconnect idx pair
// void ctl_disconnect(U32 outIdx, U32 inIdx);
