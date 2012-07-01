/* param.h
 * bees
 * aleph
 */

#ifndef _PARAM_H_ 
#define _PARAM_H_

#include "../common/types.h"
#include "net_protected.h"

// get value for param at given idx
f32 get_param_value(u32 idx);
// set value for param at given idx
void set_param_value(u32 idx, f32 val);
// get preset flag for param at given idx
u8 get_param_preset(u32 idx);

#endif // header guard
