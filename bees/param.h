/* param.h
 * bees
 * aleph
 */

#ifndef _PARAM_H_ 
#define _PARAM_H_

#include "types.h"
#include "net_protected.h"

#define PARAM_BITS 16
#define PARAM_MAX 32767
#define PARAM_MAX_F 32767.f
#define PARAM_MAX_RF 3.0517578125e-05
#define PARAM_MIN -32768
 
// get value for param at given idx
f32 get_param_value(u32 idx);
// set value for param at given idx
//void set_param_value(u32 idx, f32 val);
///// use integer and scale
void set_param_value(u32 idx, s32 val);
// get preset flag for param at given idx
u8 get_param_preset(u32 idx);

#endif // header guard
