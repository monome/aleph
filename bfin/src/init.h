#ifndef _ALEPH_BFIN_INIT_H_
#define _ALEPH_BFIN_INIT_H_

//----------- defines
// addresses for Port A in Flash A
// PA0 -> reset pin for ad1836
//#define pFlashA_PortA_Dir	(volatile unsigned char *)0x20270006
//#define pFlashA_PortA_Data	(volatile unsigned char *)0x20270004
// addresses for Port B in Flash A
// [PB0, PB05] -> [LED1, LED6]
//#define pFlashA_PortB_Dir	(volatile unsigned char *)0x20270007
//#define pFlashA_PortB_Data	(volatile unsigned char *)0x20270005

//// inputs:
/*
// buttons : pf08-11
#define PF_IN 0x0f00
// edge-sensitive
#define PF_EDGE 0x0f00
// both rise and fall
#define PF_BOTH 0x0f00
// set interrupt mask
#define_PF_IMASK  0x0f00
*/
// nothing:
#define PF_IN 0
#define PF_EDGE 0
// both rise and fall
#define PF_BOTH 0
// set interrupt mask
#define PF_IMASK  0
//// OUTPUTS:
// led3, led4 : pf14,15 
// codec reset : pf1
#define PF_DIR           0xc002 
#define CODEC_RESET_MASK 0xfffd 
#define CODEC_SS_MASK    0xffdf
// ( led masks are in leds.h so these must synchronize )

/* // names for codec registers, used for sCodec1836TxRegs[] */
/* #define DAC_CONTROL_1		0x0000 */
/* #define DAC_CONTROL_2		0x1000 */
/* #define DAC_VOLUME_0		0x2000 */
/* #define DAC_VOLUME_1		0x3000 */
/* #define DAC_VOLUME_2		0x4000 */
/* #define DAC_VOLUME_3		0x5000 */
/* #define DAC_VOLUME_4		0x6000 */
/* #define DAC_VOLUME_5		0x7000 */
/* #define ADC_0_PEAK_LEVEL	0x8000 */
/* #define ADC_1_PEAK_LEVEL	0x9000 */
/* #define ADC_2_PEAK_LEVEL	0xA000 */
/* #define ADC_3_PEAK_LEVEL	0xB000 */
/* #define ADC_CONTROL_1		0xC000 */
/* #define ADC_CONTROL_2		0xD000 */
/* #define ADC_CONTROL_3		0xE000 */

/* // size of array sCodec1836TxRegs */
/* #define CODEC_1836_REGS_LENGTH	11 */
/* // new codec: */
/* #define CODEC_1938_REGS_LENGTH	11 */

// command byte for setting codec regs (4 << 1 | 1)
#define CODEC_CMD_BYTE 0x9
// control registers for AD1938
#define PLL_CLOCK_0         0
#define PLL_CLOCK_1         1
#define DAC_CONTROL_0       2
#define DAC_CONTROL_1       3
#define DAC_CONTROL_2       4
#define DAC_MUTE            5
#define DAC_L1_VOLUME       6
#define DAC_R1_VOLUME       7
#define DAC_L2_VOLUME       8
#define DAC_R2_VOLUME       9
#define DAC_L3_VOLUME       10
#define DAC_R3_VOLUME       11
#define DAC_L4_VOLUME       12
#define DAC_R4_VOLUME       13
#define ADC_CONTROL_0       14
#define ADC_CONTROL_1       15
#define ADC_CONTROL_2       16
#define CODEC_1939_NUM_REGS 17


// SPI transfer mode
//// DMA-driven
#define TIMOD_DMA_TX 0x0003
#define TIMOD_DMA_RX 0x0002
//// buffer-driven
#define TIMOD_BUF_TX 0x0001

// SPORT0 word length
#define SLEN_24	0x0017

// DMA flow mode
#define FLOW_1 	0x1000

//--------- function prototypes
void init_clocks(void);
void init_EBIU(void);
void init_flash(void);
void init_1836(void);
void init_1939(void);
void init_spi_slave(void);
void init_sport0(void);
void init_DMA(void);
void init_interrupts(void);
void enable_DMA_sport0(void);
void init_flags(void);

#endif // header guard
