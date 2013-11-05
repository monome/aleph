#ifndef _ALEPH_BFIN_INIT_H_
#define _ALEPH_BFIN_INIT_H_

#include "gpio.h"
//----------- defines


////---- gpio
// no gpio inputs:
/* #define PF_IN 0 */
/* #define PF_EDGE 0 */
/* // both rise and fall */
/* #define PF_BOTH 0 */
/* // set interrupt mask */
/* #define PF_IMASK  0 */
//// OUTPUTS:
// led3, led4 : pf14,15 
// codec reset : pf1
// busy pin (shared with HWAIT) : pf2
//#define PF_DIR           0xc002 
//#define PF_DIR           0xc003
//#define PF_DIR (CODEC_RESET_UNMASK) | (DAC_RESET_UNMASK) | (READY_UNMASK) | (LED3_UNMASK) | (LED4_UNMASK)
//#define CODEC_RESET_MASK 0xfffd 
//#define CODEC_SS_MASK    0xffdf

// command byte for setting codec regs (4 << 1 | 1)
#define CODEC_CMD_BYTE 0x9

// control registers for AD1939
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
void init_sport1(void);
void init_DMA(void);
void init_interrupts(void);
void enable_DMA_sport0(void);
void enable_DMA_sport1(void);
void init_flags(void);

#endif // header guard
