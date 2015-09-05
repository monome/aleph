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
#include "op_derived.h"
#include "param_scaler.h"
#include "types.h"
#include "util.h"

#if NET_USE_MALLOC
#else
// size of operator pool in bytes
#define NET_OP_POOL_SIZE 0x40000 // 256K
//#define NET_OP_POOL_SIZE 0x20000 // 128K
//#define NET_OP_POOL_SIZE 0x10000 // 64K
//#define NET_OP_POOL_SIZE 0x08000 // 32K
//#define NET_OP_POOL_SIZE 0x0c000 // 24K
//#define NET_OP_POOL_SIZE 0x04000 // 16K

#endif

// input node type
typedef struct _inode {
  // parent op index in net list
  s32 opIdx;
  // input index in parent op list
  u8 opInIdx;
  // preset inclusion flag
  //  u8 preset;
  // play inclusion flag
  u8 play;
  //} __attribute__((packed)) inode_t;
} inode_t;

// output node type (index into inode list)
typedef struct _onode {
  // preset inclusion flag
  //  u8 preset;
  // output idx in parent op's output list
  u8 opOutIdx;
  // target input idx in net list
  s16 target;
  // parent op's index in net list
  s32 opIdx;
  //} __attribute__((packed)) onode_t;
} onode_t;

// parameter I/O node
typedef struct _pnode {
  ParamDesc desc;
  ParamData data;
  ParamScaler scaler;
  /// why idx?
  //  u8 idx;
  // play inclusion flag
  /// must be separate from inputs list for large input counts!
  u8 play;
  //} __attribute__((packed)) pnode_t;
} pnode_t;


// big old class for the network
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

} ctlnet_t;

//// external variables

// pointer to network!
extern ctlnet_t* net;


// pointers to system-created ops
// encoders
extern op_enc_t* opSysEnc[4];
// function keys and footswitches
extern op_sw_t* opSysSw[6];
// adc
extern op_adc_t* opSysAdc;
// preset
extern op_preset_t* opSysPreset;

// set active
extern void net_set_active(bool v);

#endif // header guard
