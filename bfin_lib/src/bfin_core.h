/* bfin_core.h
 * aleph-blackfin audio core
 *
 * basically global variable declarations 
 */

#ifndef _ALEPH_BFIN_CORE_H_
#define _ALEPH_BFIN_CORE_H_ 

#include <sys/exception.h>
#include <fract.h>
#include <fract_typedef.h>
#include "types.h"
#include "protocol.h"

// names for slots in ad1836 audio frame
#define INTERNAL_ADC_L0			0
#define INTERNAL_ADC_R0			2
#define INTERNAL_DAC_L0			0
#define INTERNAL_DAC_R0			2
#define INTERNAL_ADC_L1			1
#define INTERNAL_ADC_R1			3
#define INTERNAL_DAC_L1			1
#define INTERNAL_DAC_R1			3

//------- global variables
// per-channel audio i/o
extern fract32 in[4];
extern fract32 out[4];

// ctl change 

// audio DMA registers
extern volatile s32 iRxBuf[];
extern volatile s32 iTxBuf[];

extern volatile u32 cvTxBuf;


// audio processing flag
extern volatile u8 processAudio;

#endif // header guard
