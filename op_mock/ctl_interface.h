/* ctl_interface.h
 * aleph-avrr32
 *
 * public interface functions for using and manipulating the controller network
 */

#ifndef _CTL_INTERFACE_H_
#define _CTL_INTERFACE_H_

#include "compiler.h"

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

// initialize the network 
void ctl_net_init(void);
// activate a node with some input data
void ctl_go(U16 nodeIdx, const S32* val);
 
// get string for operator at given idx
const char* const ctl_op_name(const U8 idx);
// create a new operator given class ID, return index (-1 == fail)
S16 ctl_add_op(opid_t opId);

// get current count of operators
const U16 ctl_num_ops(void);
// get current count of inputs
const U16 ctl_num_ins(void);
// get current count of outputs
const U16 ctl_num_outs(void);

//void ctl_remove_op(const U8 idx);
//void ctl_connect(U32 outIdx, U32 inIdx);
//void ctl_disconnect(U32 outIdx, U32 inIdx);

#endif // header guard
