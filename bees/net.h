/* net.h
 * bees
 * aleph 
 *
 * public interface functions for using and manipulating the controller network
 */

#ifndef _CTL_INTERFACE_H_
#define _CTL_INTERFACE_H_

// maximum allocated parameters, connections, and operators
// max operator inputs
#define NET_INS_MAX 128
// max opertor outputs
#define NET_OUTS_MAX 128
// max  operators
#define NET_OPS_MAX 48
// max DSP parameter inputs
#define NET_PARAMS_MAX 64
// max presetsbbf
#define NET_PRESETS_MAX 8

#include "param_common.h"
#include "param.h"
// parameter data
#include "types.h"
#include "op.h"
#include "op_math.h"

//---- public functions

// initialize the network 
void net_init(void);
// de-initialize the network 
void net_deinit(void);
// intialize (clear) an input node
void net_init_inode(u16 idx);
// intialize (clear) an output node
void net_init_onode(u16 idx);
// create a new operator given class ID, return index (-1 == fail)
s16 net_add_op(opId_t opId);
// remove the last created operator
s16 net_pop_op(void);
// remove an arbitrary operator
void net_remove_op(const u32 idx);

// activate an input node with some input data
void net_activate(s16 inIdx, const io_t val);

// get current count of operators
u16 net_num_ops(void);
// get current count of inputs
u16 net_num_ins(void);
// get current count of outputs
u16 net_num_outs(void);
// get param index given index
s16 net_param_idx(u16 inIdx);
// get num params (subset of inputs)
u16 net_num_params(void);

// get string for operator at given idx
const char* net_op_name(const s16 idx);
// get name for input at given idx
const char* net_in_name(u16 idx);
// get name for output at given idx
const char* net_out_name(const u16 idx);

// get op index for input at given idx
s16 net_in_op_idx(const u16 idx);
// get op index for output at given idx
s16 net_out_op_idx(const u16 idx);
// get global index for a given input of given op
u16 net_op_in_idx(const u16 opIdx, const u16 inIdx);
// get global index for a given output of given op
u16 net_op_out_idx(const u16 opIdx, const u16 outIdx);
// get connection index for output
s16 net_get_target(u16 outIdx);
// is this input connected to anything?
u8 net_in_connected(s32 iIdx);
// get status (user/system) of op at given idx
opStatus_t net_op_status(u16 opIdx);

// get / set / increment input value
io_t net_get_in_value(s32 inIdx);
void net_set_in_value(s32 inIdx, io_t val);
io_t net_inc_in_value(s32 inIdx, io_t inc);

// connect a given output and input idx pair
void net_connect(u32 outIdx, u32 inIdx);
// disconnect a given output
void net_disconnect(u32 outIdx);

// toggle preset inclusion for input
u8 net_toggle_in_preset(u32 inIdx);
// toggle preset inclusion for output
u8 net_toggle_out_preset(u32 outIdx);
// set preset inclusion for input
void net_set_in_preset(u32 inIdx, u8 val);
// set preset inclusion for output
void net_set_out_preset(u32 outIdx, u8 val);
// get preset inclusion for input
u8 net_get_in_preset(u32 inIdx);
// get preset inclusion for output
u8 net_get_out_preset(u32 outIdx);
// add a new parameter
void net_add_param(u32 idx, volatile ParamDesc* pdesc);
//void net_add_param(u32 idx, const char* name, f32 min, f32 max, s32 val);
// clear existing parameters
void net_clear_params(void);


// forward/backward search for input/output nodes
s32 net_next_in(void);
s32 net_prev_in(void);
s32 net_next_out(void);
s32 net_prev_out(void);
void net_reset_in_search(void);
void net_reset_out_search(void);

// populate an array with indices of all connected outputs for a given index
// returns count of connections
u32 net_gather(s32 iIdx, u32(*outs)[NET_OUTS_MAX]);

#endif // header guard
