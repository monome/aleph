/* bf533_audio_core.h
 * aleph-blackfin audio core
 *
 */

#ifndef __BF533_CORE_H__
#define __BF533_CORE_H__

#include <sys/exception.h>
#include <cdefBF533.h>
#include "../../common/protocol.h"
#include "module.h"


// names for slots in ad1836 audio frame
#define INTERNAL_ADC_L0			0
#define INTERNAL_ADC_R0			2
#define INTERNAL_DAC_L0			0
#define INTERNAL_DAC_R0			2
#define INTERNAL_ADC_L1			1
#define INTERNAL_ADC_R1			3
#define INTERNAL_DAC_L1			1
#define INTERNAL_DAC_R1			3

// size of array sCodec1836TxRegs
#define CODEC_1836_REGS_LENGTH	11

// SPI transfer mode
#define TIMOD_DMA_TX 0x0003
#define TIMOD_DMA_RX 0x0002

// SPORT0 word length
#define SLEN_24	0x0017

// DMA flow mode
#define FLOW_1	0x1000

//------- global variables
// per-channel audio i/o
extern int iIn00;
extern int iIn01;
extern int iIn10;
extern int iIn11;
extern int iOut00;
extern int iOut01;
extern int iOut10;
extern int iOut11;

// ad1836 configuration register
extern volatile short sCodec1836TxRegs[];
// audio DMA registers
extern volatile int iRxBuf[];
extern volatile int iTxBuf[];

// flag indicating spi rx data needs dealing with
extern unsigned char spiRxFlag;
// ringbuffer for dealing with spi rx data outside the ISR
extern unsigned short int spiRxRing[P_PARAM_MSG_WORD_COUNT];
// counter for indexing spi ringbufer
extern unsigned short int spiRxRingIdx;

/*
// ringbuffer for audio processing
extern int audioRing[AUDIO_RINGBUFFER_N]
// I/O counters for audio ringbuffer
// input counter
extern unsigned int audioRingIdxI = 0;
// output counter
extern unsigned int audioRingIdxO = 9;
*/
extern volatile unsigned char needAudioFrame;

//----- in file isr.c
// sport0 receive interrupt (ad1836)
void sport0_rx_isr() __attribute((interrupt_handler));
// spi data receive interrupt (avr32)
void spi_rx_isr() __attribute__((interrupt_handler));


#endif // header guard
