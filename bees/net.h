/* net.h
 * bees
 * aleph 
 *
 * public interface functions for using and manipulating the controller network
 */

#ifndef _CTL_INTERFACE_H_
#define _CTL_INTERFACE_H_

#include "types.h"
#include "op.h"

// maximum allocated parameters, connections, and operators
// mac operator inputs
#define NET_INS_MAX 256
// max opertor outputs
#define NET_OUTS_MAX 256
// max  operators
#define NET_OPS_MAX 256
// max DSP parameter inputs
#define NET_PARAMS_MAX 256
// max presets
#define NET_PRESETS_MAX 256

//---- public functions

// initialize the network 
void net_init(void);
// de-initialize the network 
void net_deinit(void);
// create a new operator given class ID, return index (-1 == fail)
S16 net_add_op(opId_t opId);
// remove the last created operator
S16 net_pop_op(void);
// remove an arbitrary operator
// TODO
// void remove_op(const U8 idx);

// activate an input node with some input data
void net_activate(S16 inIdx, const S32 val);

// get current count of operators
U16 net_num_ops(void);
// get current count of inputs
U16 net_num_ins(void);
// get current count of outputs
U16 net_num_outs(void);
// get param index given index
S16 net_param_idx(U16 inIdx);

// get string for operator at given idx
const char* net_op_name(const S16 idx);
// get name for input at given idx
const char* net_in_name(U16 idx);
// get name for output at given idx
const char* net_out_name(const U16 idx);

// get op index for input at given idx
S16 net_in_op_idx(const U16 idx);
// get op index for output at given idx
S16 net_out_op_idx(const U16 idx);
// get global index for a given input of given op
U16 net_op_in_idx(const U16 opIdx, const U16 inIdx);
// get global index for a given output of given op
U16 net_op_out_idx(const U16 opIdx, const U16 outIdx);
// get connection index for output
S16 net_get_target(U16 outIdx);
// is this input connected to anything?
U8 net_in_connected(U32 iIdx);
// get status (user/system) of op at given idx
opStatus_t net_op_status(U16 opIdx);

// get / set / increment input value
s32 net_get_in_value(U16 inIdx);
void net_set_in_value(U16 inIdx, S32 val);
s32 net_inc_in_value(s16 inIdx, S32 inc);

// connect a given output and input idx pair
void net_connect(U32 outIdx, U32 inIdx);
// disconnect a given output
void net_disconnect(U32 outIdx);

// toggle preset inclusion for input
u8 net_toggle_in_preset(U32 inIdx);
// toggle preset inclusion for output
u8 net_toggle_out_preset(U32 outIdx);
// get preset inclusion for input
U8 net_get_in_preset(U32 inIdx);
// get preset inclusion for output
U8 net_get_out_preset(U32 outIdx);
// add a new parameter
void net_add_param(u32 idx, const char* name, f32 min, f32 max, s32 val);
// clear existing parameters
void net_clear_params(void);


// populate an array with indices of all connected outputs for a given index
// returns count of connections
U32 net_gather(U32 iIdx, U32(*outs)[NET_OUTS_MAX]);

#endif // header guard
