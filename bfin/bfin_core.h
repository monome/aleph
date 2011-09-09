/* bfin_core.h
 * aleph-blackfin audio core
 *
 * basically global variable declarations 
 */

#ifndef _ALEPH_BFIN_CORE_H_
#define _ALEPH_BFIN_CORE_H_ 

//#include <sys/exception.h>
//#include <cdefBF533.h>
#include <sys/exception.h>
#include <cdefBF533.h>
#include <fract.h>
#include <fract_typedef.h>
#include "types.h"
#include "../../common/protocol.h"

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
extern fract32 in00;
extern fract32 in01;
extern fract32 in10;
extern fract32 in11;
extern fract32 out00;
extern fract32 out01;
extern fract32 out10;
extern fract32 out11;

// audio DMA registers
extern volatile s32 iRxBuf[];
extern volatile s32 iTxBuf[];

#endif // header guard
