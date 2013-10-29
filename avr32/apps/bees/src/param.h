/* param.h
 * bees
 * aleph
 */

#ifndef _PARAM_H_ 
#define _PARAM_H_

// common
#include "param_common.h"
#include "types.h"
// bees
#include "net_protected.h"
#include "op_math.h"

#define PARAM_BITS 16
#define PARAM_MAX 32767
#define PARAM_MAX_F 32767.f
#define PARAM_MAX_RF 3.0517578125e-05
#define PARAM_MIN -32768
 
// get value for param at given idx
io_t get_param_value(u32 idx);
// get preset flag for param at given idx
u8 get_param_preset(u32 idx);
// get name
const char* get_param_name(u32 idx);


// set value with default numerical representation 
void set_param_value(u32 idx, io_t val);
// set value with float
// void set_param_float_value(u32 idx, f32 val);
// set value, full-scale int
// void set_param_step_value(u32 idx, s32 val);

/* // copy descriptor */
/* void param_desc_copy(ParamDesc* dst, ParamDesc* src);  */

// get pointer to descriptor
//const ParamDesc* getParamDesc(); 

// get param descriptors and initial values from DSP
void report_params(void);

//---- pickle / unpickle

// node 
extern u8* param_pickle(   pnode_t* pnode, u8* dst);
extern const u8* param_unpickle( pnode_t* pnode, const u8* src);
// descriptor
extern u8* pdesc_pickle(ParamDesc* pdesc, u8* dst);
extern const u8* pdesc_unpickle(ParamDesc* pdesc, const u8* src);


#endif // header guard
