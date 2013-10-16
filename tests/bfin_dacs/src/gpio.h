#ifndef _ALEPH_BFIN_GPIO_H_
#define _ALEPH_BFIN_GPIO_H_

// leds are on pf14, pf15
#define LED3_MASK     0x7fff 
#define LED3_UNMASK   0x8000 
#define LED4_MASK     0xbfff
#define LED4_UNMASK   0x4000

#define LED3_SET    	*pFIO_FLAG_D &= LED3_MASK
#define LED3_UNSET  	*pFIO_FLAG_D |= LED3_UNMASK
#define LED3_TOGGLE 	*pFIO_FLAG_D ^= LED3_UNMASK

#define LED4_SET	*pFIO_FLAG_D &= LED4_MASK
#define LED4_UNSET 	*pFIO_FLAG_D |= LED4_UNMASK
#define LED4_TOGGLE	*pFIO_FLAG_D ^= LED4_UNMASK

#define BUSY_MASK 	0xfffb
#define BUSY_UNMASK	0x0004
#define BUSY_SET 	*pFIO_FLAG_D |= BUSY_UNMASK
#define BUSY_UNSET 	*pFIO_FLAG_D &= BUSY_MASK

#define LDAC_UNMASK 	1 << 7
#define LDAC_MASK 	0xffff ^ LDAC_UNMASK
#define LDAC_SET 	*pFIO_FLAG_D |= LDAC_UNMASK
#define LDAC_UNSET 	*pFIO_FLAG_D &= LDAC_MASK

#endif // h guard
