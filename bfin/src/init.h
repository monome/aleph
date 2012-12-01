#ifndef _ALEPH_BFIN_INIT_H_
#define _ALEPH_BFIN_INIT_H_

//----------- defines
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

// size of array sCodec1836TxRegs
#define CODEC_1836_REGS_LENGTH	11

// SPI transfer mode
#define TIMOD_DMA_TX 0x0003
#define TIMOD_DMA_RX 0x0002

// SPORT0 word length
#define SLEN_24	0x0017

// DMA flow mode
#define FLOW_1	0x1000

// SDRAM settings
//#define SDRAM_ROWS
 
//--------- function prototypes
void init_clocks(void);
void init_EBIU(void);
void init_flash(void);
void init_1836(void);
void init_spi_slave(void);
void init_sport0(void);
void init_DMA(void);
void init_interrupts(void);
void enable_DMA_sport0(void);
void init_flags(void);

#endif // header guard
