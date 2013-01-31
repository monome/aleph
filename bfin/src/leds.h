#ifndef _ALEPH_BFIN_LEDS_H_
#define _ALEPH_BFIN_LEDS_H_

// leds are on pf14, pf15
#define LED3_MASK     0x7fff 
#define LED3_UNMASK   0x8000 
#define LED4_MASK     0xbfff
#define LED4_UNMASK   0x4000

#define SET_LED3    *pFIO_FLAG_D &= LED3_MASK
#define UNSET_LED3  *pFIO_FLAG_D |= LED3_UNMASK
#define SET_LED4    *pFIO_FLAG_D &= LED4_MASK
#define UNSET_LED4  *pFIO_FLAG_D |= LED4_UNMASK

#endif // h guard
