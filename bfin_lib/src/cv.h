#ifndef _ALEPH_BFIN_DAC_H_
#define _ALEPH_BFIN_DAC_H_

#include "bfin_core.h"

#define CV_DAC_VALUE_MASK 0xffff

// initialize CV dac
extern void init_cv(void);

// update given CV channel. 
// input is assumed to be in [0, 0x7fffffff]
extern void cv_update(u8 ch, fract32 val);

#endif // h guard
