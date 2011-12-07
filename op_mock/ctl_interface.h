/* ctl_interface.h
 * aleph-avrr32
 *
 * public interface functions for using and manipulating the controller network
 */

#ifndef _CTL_INTERFACE_H_
#define _CTL_INTERFACE_H_

#include "compiler.h"

//---- types

typedef enum {
  eOpSwitch,
  eOpEnc,
  eOpAdd,
  eOpMul,
  eOpGate,
  eOpAccum,
  eOpSelect,
  eOpMapLin,
  eOpParam,
  eOpPreset,
  numOpClasses
} opid_t;

typedef struct op_desc_struct {
  const char* name;
  const U32 size;
} op_desc_t;

//----- variables

extern op_desc_t op_registry[numOpClasses];

//---- public functions

// initialize the network 
void ctl_net_init(void);
// create a new operator given class ID, return index (-1 == fail)
S16 ctl_add_op(opid_t opId);
// activate a node with some input data
void ctl_go(U16 nodeIdx, const S32* val);
// get current count of operators
U8 ctl_num_ops(void);
// get current count of inputs
U8 ctl_num_ins(void);
// get current count of outputs
U8 ctl_num_outs(void);

// get string for operator at given idx
const char* ctl_op_name(const U8 idx);
// get name for input at given idx
const char* ctl_in_name(const U8 idx);
// get name for output at given idx
const char* ctl_out_name(const U8 idx);
// get op index for input at given idx
U8 ctl_in_op_idx(const U8 idx);
// get op index for output at given idx
U8 ctl_out_op_idx(const U8 idx);

//void ctl_remove_op(const U8 idx);
//void ctl_connect(U32 outIdx, U32 inIdx);
//void ctl_disconnect(U32 outIdx, U32 inIdx);

#endif // header guard
