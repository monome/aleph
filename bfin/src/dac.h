#ifndef _ALEPH_BFIN_DAC_H_
#define _ALEPH_BFIN_DAC_H_

#include "bfin_core.h"

#define DAC_COM_WRITE 0x3
#define DAC_COM_LSHIFT 20
#define DAC_ADDR_LSHIFT 16

#define DAC_VALUE_MASK 0x7fff

extern void init_dac(void);
extern void dac_update(u8 ch, u16 val);

#endif // h guard
