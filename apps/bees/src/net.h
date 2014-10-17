/* net.h
 * bees
 * aleph 
 *
 * public interface functions for using and manipulating the controller network
 */

#ifndef _CTL_INTERFACE_H_
#define _CTL_INTERFACE_H_

////////////////////////////////
//////  uhh it is easiest to just put these here. sorry. suggestions welcome...

// maximum allocated parameters, connections, and operators
//// FIXME: need to fix malloc(),
//  and also stream scene data to the card rather than having a huge serial blob.
// max operator inputs
#define NET_INS_MAX 256
// max operator outputs
#define NET_OUTS_MAX 256
// max  operators
#define NET_OPS_MAX 128
// max DSP parameter inputs
#define NET_PARAMS_MAX 256
// max presets
#define NET_PRESETS_MAX 32

///////////////////////////////////////////

#include "param_common.h"
//#include "param.h"
// parameter data
#include "types.h"
#include "op.h"
#include "op_math.h"
//---- public functions

// initialize the network 
extern void net_init(void);

// de-initialize the network 
extern void net_deinit(void);

// intialize (clear) an input node
extern void net_init_inode(u16 idx);

// intialize (clear) an output node
extern void net_init_onode(u16 idx);

// create a new operator given class ID, return index (-1 == fail)
extern s16 net_add_op(op_id_t opId);

// remove the last created operator
extern s16 net_pop_op(void);

// remove an arbitrary operator
// FIXME: not tested really... use at your own risk
extern void net_remove_op(const u32 idx);

// activate an input node with some input data
extern void net_activate(s16 inIdx, const io_t val, void* srcOp);

// get current count of operators
extern u16 net_num_ops(void);

// get current count of inputs (including dsp parameters!)
extern u16 net_num_ins(void);

// get current count of outputs
extern u16 net_num_outs(void);

// get param index given global index
extern s16 net_param_idx(u16 inIdx);

// get num params (subset of inputs)
extern u16 net_num_params(void);

// get string for operator at given idx
extern const char* net_op_name(const s16 idx);

// get name for input at given idx
extern const char* net_in_name(u16 idx);

// get name for output at given idx
extern const char* net_out_name(const u16 idx);

// get op index for input at given idx
extern s16 net_in_op_idx(const u16 idx);

// get op index for output at given idx
extern s16 net_out_op_idx(const u16 idx);

// get global index for a given input of given op
//// NOTE: this is relatively slow, try not to use in realtime
extern u16 net_op_in_idx(const u16 opIdx, const u16 inIdx);

// get global index for a given output of given op
extern u16 net_op_out_idx(const u16 opIdx, const u16 outIdx);

// get connection index for output
extern s16 net_get_target(u16 outIdx);

// is this input connected to anything?
extern u8 net_in_connected(s32 iIdx);

// get true/false for given op behavior flag
// (see op.h)
extern u8 net_op_flag(const u16 opIdx, op_flag_t flag);

// get / set / increment input value
extern io_t net_get_in_value(s32 inIdx);
extern void net_set_in_value(s32 inIdx, io_t val);
extern io_t net_inc_in_value(s32 inIdx, io_t inc);


// connect a given output and input idx pair
extern void net_connect(u32 outIdx, u32 inIdx);

// disconnect a given output
extern void net_disconnect(u32 outIdx);

// toggle preset inclusion for input
extern u8 net_toggle_in_preset(u32 inIdx);

// toggle preset inclusion for output
extern u8 net_toggle_out_preset(u32 outIdx);

// set preset inclusion for input
extern void net_set_in_preset(u32 inIdx, u8 val);

// set preset inclusion for output
extern void net_set_out_preset(u32 outIdx, u8 val);

// get preset inclusion for input
extern u8 net_get_in_preset(u32 inIdx);

// get preset inclusion for output
extern u8 net_get_out_preset(u32 outIdx);

// toggle play inclusion for input
extern u8 net_toggle_in_play(u32 inIdx);

// set play inclusion for input
extern void net_set_in_play(u32 inIdx, u8 val);

// get play inclusion for input 
extern u8 net_get_in_play(u32 inIdx);

// add a new parameter
extern void net_add_param(u32 idx, const ParamDesc* pdesc);

// clear existing parameters
extern void net_clear_params(void);

// resend all parameter values
extern void net_send_params(void);

// get parameter string representations,
// given string buffer and index in inputs list
extern void net_get_param_value_string(char* dst, u32 idx);
// same, with arbitrary value
extern void net_get_param_value_string_conversion(char* dst, u32 idx, s32 val);


// retrigger all inputs
extern void net_retrigger_ins(void);

// populate an array with indices of all connected outputs for a given index
// returns count of connections
extern u32 net_gather(s32 iIdx, u32(*outs)[NET_OUTS_MAX]);

// query the blackfin for parameter list and populate pnodes
/// this is now populated from offline descriptor file
// extern u8 net_report_params(void);

// pickle the network!
// return incremented pointer to dst
extern u8* net_pickle(u8* dst);

// unpickle the network!
// return incremented pointer to src
extern u8* net_unpickle(const u8* src);

// clear ops and i/o
extern void net_clear_user_ops(void);

// disconnect from parameters
extern void net_disconnect_params(void);

// insert a split after an output node
// return out11 of split if original out was unconnected,
// otherwise connect out1 of split to old target and return out2
extern s16 net_split_out(s16 outIdx);

// report whether given input is DSP param
//  extern bool net_in_is_dsp(s16 inIdx);

/// test/dbg
void net_print(void);

#endif // header guard


