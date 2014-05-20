#define __DEFINED_SSYNC
#define __DEFINED_CSYNC
#include <cdefBF533.h>

#ifndef _ALEPH_BFIN_GPIO_H_
#define _ALEPH_BFIN_GPIO_H_

// codec reset
#define CODEC_RESET_PIN	   1
#define CODEC_RESET_UNMASK (1 << CODEC_RESET_PIN)
#define CODEC_RESET_MASK   (0xffff ^ CODEC_RESET_UNMASK)
#define CODEC_RESET_LO	   *pFIO_FLAG_D &= CODEC_RESET_MASK
#define CODEC_RESET_HI     *pFIO_FLAG_D |= CODEC_RESET_UNMASK

// cv dac reset 
#define CV_DAC_RESET_PIN	 6
#define CV_DAC_RESET_UNMASK 	(1 << CV_DAC_RESET_PIN)
#define CV_DAC_RESET_MASK   	(0xffff ^ CV_DAC_RESET_UNMASK)
#define CV_DAC_RESET_LO	 	*pFIO_FLAG_D &= CV_DAC_RESET_MASK
#define CV_DAC_RESET_HI		*pFIO_FLAG_D |= CV_DAC_RESET_UNMASK

// dac LDAC
#define CV_DAC_LDAC_PIN	 	7
#define CV_DAC_LDAC_UNMASK 	(1 << CV_DAC_LDAC_PIN)
#define CV_DAC_LDAC_MASK   	(0xffff ^ CV_DAC_LDAC_UNMASK)
#define CV_DAC_LDAC_LO	 	*pFIO_FLAG_D &= CV_DAC_LDAC_MASK
#define CV_DAC_LDAC_HI		*pFIO_FLAG_D |= CV_DAC_LDAC_UNMASK

// "ready" pin - pf12
// set high when able to receive param changes
// e.g. init finished, frame finished
#define READY_PIN        13
#define READY_UNMASK 	(1 << READY_PIN)
#define READY_MASK	(0xffff ^ READY_UNMASK)
#define READY_LO 	*pFIO_FLAG_D &= READY_MASK
#define READY_HI 	*pFIO_FLAG_D |= READY_UNMASK

// "request" pin - pf13
// should it prove necessary, bfin will use this pin
// to trigger interrupts on avr32.
#define REQUEST_PIN        12
#define REQUEST_UNMASK 	(1 << REQUEST_PIN)
#define REQUEST_MASK	(0xffff ^ REQUEST_UNMASK)
#define REQUEST_LO 	*pFIO_FLAG_D &= REQUEST_MASK
#define REQUEST_HI 	*pFIO_FLAG_D |= REQUEST_UNMASK

// leds are on pf14, pf15
#define LED3_PIN 	14
#define LED3_UNMASK     (1 << LED3_PIN)
#define LED3_MASK       (0xffff ^ LED3_UNMASK)
#define LED3_LO		*pFIO_FLAG_D &= LED3_MASK
#define LED3_HI 	*pFIO_FLAG_D |= LED3_UNMASK
#define LED3_TOGGLE	*pFIO_FLAG_D ^= LED3_UNMASK

#define LED4_PIN 	15
#define LED4_UNMASK     (1 << LED4_PIN)
#define LED4_MASK       (0xffff ^ LED4_UNMASK)
#define LED4_LO		*pFIO_FLAG_D &= LED4_MASK
#define LED4_HI 	*pFIO_FLAG_D |= LED4_UNMASK
#define LED4_TOGGLE	*pFIO_FLAG_D ^= LED4_UNMASK


#endif // h guard
