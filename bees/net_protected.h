/* net_protected.h
 * bees
 * aleph
 *
 * private header for network types
 */

#ifndef _NET_PROTECTED_H_
#define _NET_PROTECTED_H_

#include "../common/types.h"
#include "op.h"
#include "param.h"
#include "net.h"

// size of operator pool inn bytes
#define NET_OP_POOL_SIZE 65536

// input node type (function pointer)
typedef struct _inode {
  // last value
  s32 val;
  // function pointer
  op_in_t in;
  // input idx in net list
  u16 inIdx;
  // parent op index in net list
  u16 opIdx;
  // preset inclusion flag
  u8 preset;
} inode_t;

// output node type (index into inode list)
typedef struct _onode {
  // output idx in parent op's output list
  u16 outIdx;
  // target input idx in net list
  s16 target;
  // parent op's index in net list
  u16 opIdx;
  // preset inclusion flag
  u8 preset;
} onode_t;

// parameter I/O node
typedef struct _pnode {
// min value
  f32 min;
  // max value
  f32 max;
  // current value (step)
  s32 val;  
// current value (float)
  f32 fval;
  // parameter idx in DSP plugin
  u32 idx;
  // parameter name
  const char* name;
  // plugin name
  //  const char* dspName;
  // preset flag
  u8 preset;
  // TODO:
  // scaling functions/tables ?
  // units ?
} pnode_t;

// network type
typedef struct _ctlnet {
  // operator memory, statically allocated as char array
  U8 opPoolMem[NET_OP_POOL_SIZE];
  // pointer to managed op memory
  void * opPool;
  // current offset into op memory
  U32 opPoolOffset;
  // number of instantiated operators
  U16 numOps;
  // number of instantiated inputs
  U16 numIns;
  // number of instantiated outputs
  U16 numOuts;
  // number of instantiated params
  U16 numParams;
  //  op pointers
  op_t * ops[NET_OPS_MAX];
  // inputs
  inode_t ins[NET_INS_MAX];
  // outputs
  onode_t outs[NET_OUTS_MAX];
  // DSP params
  pnode_t params[NET_PARAMS_MAX];

} ctlnet_t;

//// external variables
extern ctlnet_t net;

#endif // header guard
