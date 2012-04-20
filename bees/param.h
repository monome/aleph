/* param.h
 * bees
 * aleph
 */

#ifndef _PARAM_H_ 
#define _PARAM_H_

#include "../common/types.h"

// DSP parameter inputs
#define NET_PARAMS_MAX 256

// parameter I/O node
typedef struct pnode_struct {
  // current value
  f32 val;
  // min value
  f32 min;
  // max value
  f32 max;
  // parameter idx in DSP plugin
  u32 idx;
  // parameter name
  const char* name;
  // plugin name
  //  const char* dspName;
  // preset flag
  u8 preset;
  // TODO:
  // scaling functions/tables ?
  // units ?
} pnode_t;

// set value for param at given idx
void param_set(u32 idx, f32 val);

#endif // header guard
