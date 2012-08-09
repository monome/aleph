/* net_protected.h
 * bees
 * aleph
 *
 * private header for network types
 */

#ifndef _NET_PROTECTED_H_
#define _NET_PROTECTED_H_

#include "module_common.h"
#include "net.h"
#include "op.h"
#include "param.h"
#include "types.h"


// size of operator pool in bytes
//#define NET_OP_POOL_SIZE 65536
#define NET_OP_POOL_SIZE 8192

// input node type (function pointer)
typedef struct _inode {
  // last value
  //  s32 val;
  // function pointer
  //op_in_t in;
  // input idx in net list
  //  u16 netInIdx;
  // parent op index in net list
  u16 opIdx;
  // input index in parent op list
  u16 opInIdx;
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
  s32 ival;  
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
  u8 opPoolMem[NET_OP_POOL_SIZE];
  // pointer to managed op memory
  u8 * opPool;
  // current offset into op memory
  u32 opPoolOffset;
  // number of instantiated operators
  u16 numOps;
  // number of instantiated inputs
  u16 numIns;
  // number of instantiated outputs
  u16 numOuts;
  // number of instantiated params
  u16 numParams;
  //  op pointers
  op_t * ops[NET_OPS_MAX];
  // inputs
  inode_t ins[NET_INS_MAX];
  // outputs
  onode_t outs[NET_OUTS_MAX];
  // DSP params
  pnode_t params[NET_PARAMS_MAX];
  // module name
  char moduleName[MODULE_NAME_LEN];
} ctlnet_t;

//// external variables
extern ctlnet_t net;

#endif // header guard
