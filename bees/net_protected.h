/* net_protected.h
 * bees
 * aleph
 *
 * private header for network types
 */

#ifndef _NET_PROTECTED_H_
#define _NET_PROTECTED_H_

// use dynamically allocated memory and vectors 
#define NET_USE_MALLOC 0

#if NET_USE_MALLOC
#include <stdlib.h>
#endif

#include "module_common.h"
#include "net.h"
#include "op.h"
#include "param.h"
#include "types.h"
#include "util.h"

#if NET_USE_MALLOC
#else
// size of operator pool in bytes
#define NET_OP_POOL_SIZE 0x10000
#endif

// input node type (function pointer)
typedef struct _inode {
  // parent op index in net list
  s32 opIdx;
  // input index in parent op list
  u8 opInIdx;
  // preset inclusion flag
  u8 preset;
} inode_t;

// output node type (index into inode list)
typedef struct _onode {
  // output idx in parent op's output list2
  u8 opOutIdx;
  // target input idx in net list
  s16 target;
  // parent op's index in net list
  s32 opIdx;
  // preset inclusion flag
  u8 preset;
} onode_t;

// parameter I/O node
typedef struct _pnode {
  ParamDesc desc;
  ParamData data;
  u8 idx;
  u8 preset;
} pnode_t;


// network type
typedef struct _ctlnet {
  //  op pointers
  op_t * ops[NET_OPS_MAX];
#if NET_USE_MALLOC
  // bitfields for operator allocation
  u8 opsUsed[BITNSLOTS(NET_OPS_MAX)];
#else
  // operator memory, statically allocated as char array
  u8 opPoolMem[NET_OP_POOL_SIZE];
  // pointer to managed op memory
  u8 * opPool;
  // current offset into op memory
  u32 opPoolOffset;
#endif
  // number of instantiated operators
  u16 numOps;
  // number of instantiated inputs
  u16 numIns;
  // number of instantiated outputs
  u16 numOuts;
  // number of instantiated params
  u16 numParams;

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
extern ctlnet_t* net;

#endif // header guard
