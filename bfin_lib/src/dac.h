#ifndef _ALEPH_BFIN_DAC_H_
#define _ALEPH_BFIN_DAC_H_

#include "bfin_core.h"

#define DAC_COM_WRITE 0x3
#define DAC_COM_LSHIFT 20
#define DAC_ADDR_LSHIFT 16

#define DAC_VALUE_MASK 0xffff

// initialize CV dax
extern void init_dac(void);
// update given CV channel. 
// input is assumed to be in [0, 0x7fffffff]
extern void dac_update(u8 ch, fract32 val);

#endif // h guard
