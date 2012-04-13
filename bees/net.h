/* ctl_interface.h
 * aleph-avrr32
 *
 * public interface functions for using and manipulating the controller network
 */

#ifndef _CTL_INTERFACE_H_
#define _CTL_INTERFACE_H_

#include "../common/types.h"
#include "op.h"

// maximum allocated parameters, connections, and operators
#define CTLNET_INS_MAX 128
#define CTLNET_OUTS_MAX 128
#define CTLNET_OPS_MAX 128

//---- public functions

// initialize the network 
void net_init(void);
// de-initialize the network 
void net_deinit(void);
// create a new operator given class ID, return index (-1 == fail)
S16 net_add_op(opid_t opId);
// remove the last created operator
S16 net_pop_op(void);
// remove an arbitrary operator
// TODO
// void remove_op(const U8 idx);
// activate an input node with some input data
void net_activate(S16 inIdx, const S32* val);
// get current count of operators
U16 net_num_ops(void);
// get current count of inputs
U16 net_num_ins(void);
// get current count of outputs
U16 net_num_outs(void);
// get string for operator at given idx
const char* net_op_name(const U16 idx);
// get name for input at given idx
const char* net_in_name(const U16 idx);
// get name for output at given idx
const char* net_out_name(const U16 idx);
// get op index for input at given idx
U16 net_in_op_idx(const U16 idx);
// get op index for output at given idx
U16 net_out_op_idx(const U16 idx);
// get global index for a given input of given op
U16 net_op_in_idx(const U16 opIdx, const U16 inIdx);
// get global index for a given output of given op
U16 net_op_out_idx(const U16 opIdx, const U16 outIdx);

// get / set / increment input value
f32 net_get_in_value(U16 inIdx);
void net_set_in_value(U16 inIdx, f32 val);
f32 net_inc_in_value(U16 inIdx, f32 inc);

// get connection index for output
S16 net_get_target(U16 outIdx);

// connect a given output and input idx pair
void net_connect(U32 outIdx, U32 inIdx);
// disconnect a given output
void net_disconnect(U32 outIdx);

// populate an array with indices of all connected outputs for a given index
// returns count of connections
U32 net_gather(U32 iIdx, U32(*outs)[CTLNET_OUTS_MAX]);

// is this input connected to anything?
U8 net_in_connected(U32 iIdx);

#endif // header guard
