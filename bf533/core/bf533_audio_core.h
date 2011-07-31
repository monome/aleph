/* bf533_audio_core.h
 * aleph-blackfin audio core
 *
 */

#ifndef __BF533_CORE_H__
#define __BF533_CORE_H__

#include <sys/exception.h>
#include <cdefBF533.h>
#include "../../common/protocol.h"

// addresses for Port A in Flash A
// PA0 -> reset pin for ad1836
#define pFlashA_PortA_Dir	(volatile unsigned char *)0x20270006
#define pFlashA_PortA_Data	(volatile unsigned char *)0x20270004
// addresses for Port B in Flash A
// [PB0, PB05] -> [LED1, LED6]
#define pFlashA_PortB_Dir	(volatile unsigned char *)0x20270007
#define pFlashA_PortB_Data	(volatile unsigned char *)0x20270005

// names for codec registers, used for sCodec1836TxRegs[]
#define DAC_CONTROL_1		0x0000
#define DAC_CONTROL_2		0x1000
#define DAC_VOLUME_0		0x2000
#define DAC_VOLUME_1		0x3000
#define DAC_VOLUME_2		0x4000
#define DAC_VOLUME_3		0x5000
#define DAC_VOLUME_4		0x6000
#define DAC_VOLUME_5		0x7000
#define ADC_0_PEAK_LEVEL	0x8000
#define ADC_1_PEAK_LEVEL	0x9000
#define ADC_2_PEAK_LEVEL	0xA000
#define ADC_3_PEAK_LEVEL	0xB000
#define ADC_CONTROL_1		0xC000
#define ADC_CONTROL_2		0xD000
#define ADC_CONTROL_3		0xE000

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

// ad1836 cnofiguration register
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

//--------- function prototypes
//----- in file init.c
void init_EBIU(void);
void init_flash(void);
void init_1836(void);
void init_spi_slave(void);
void init_sport0(void);
void init_DMA(void);
void init_interrupts(void);
void enable_DMA_sport0(void);

//----- in file isr.c
// sport0 receive interrupt (ad1836)
void sport0_rx_isr() __attribute((interrupt_handler));
// spi data receive interrupt (avr32)
void spi_rx_isr() __attribute__((interrupt_handler));

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ------ defined in custom module sources!
// frame processing function
void process_frame(void);
// set parameter functions
// scaled to linear range with unsigned int, variable bit depth
void set_param_int(unsigned int idx, unsigned int val, unsigned int bitDepth);
// direct set with float
void set_param_float(unsigned int idx, float val);
#endif // header guard
