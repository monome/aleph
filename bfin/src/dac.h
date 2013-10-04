#ifndef _ALEPH_BFIN_DAC_H_
#define _ALEPH_BFIN_DAC_H_

#include "bfin_core.h"

#define DAC_RESET_PIN 6

#define DAC_COM_WRITE 0x3
#define DAC_COM_LSHIFT 20
#define DAC_ADDR_LSHIFT 16

/* #define DAC_RESET_MASK     0x7fff  */
/* #define DAC_RESET_UNMASK   0x8000  */

/* #define SET_DAC_RESET    *pFIO_FLAG_D &= DAC_RESET_MASK */
/* #define UNSET_DAC_RESET  *pFIO_FLAG_D |= DAC_RESET_UNMASK */

extern void init_dac(void);
extern void dac_update(u8 ch, u16 val);


#endif // h guard
