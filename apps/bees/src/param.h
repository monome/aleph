/* param.h
 * bees
 * aleph
 */


//////
/////  
/* NOTE: pnode_t is defined in net_protected.h, not here
this is to avoid some annoying issues of inclusion order.
*/

#ifndef _PARAM_H_ 
#define _PARAM_H_

// common
#include "param_common.h"
#include "types.h"
// bees
#include "net_protected.h"
#include "op_math.h"

// this should be syncronized with pdesc_pickle/unpickle!
// FIXME: also, it should be word-aligned...
// label, type, min, max, radix
#define PARAM_DESC_PICKLE_BYTES (PARAM_LABEL_LEN + 4 + 4 + 4 + 4)

// get value for param at given idx
extern io_t get_param_value(u32 idx);
// get preset flag for param at given idx
extern u8 get_param_preset(u32 idx);
// get name
extern const char* get_param_name(u32 idx);
// fill buffer with readable value string
extern void get_param_string(char* dst, u32 idx);

// set value
extern void set_param_value(u32 idx, io_t val);
// increment value
extern io_t inc_param_value(u32 idx, io_t inc);

//---- pickle / unpickle
// node 
extern u8* param_pickle(   pnode_t* pnode, u8* dst);
extern const u8* param_unpickle( pnode_t* pnode, const u8* src);
// descriptor
extern u8* pdesc_pickle(ParamDesc* pdesc, u8* dst);
extern const u8* pdesc_unpickle(ParamDesc* pdesc, const u8* src);


#endif // header guard
